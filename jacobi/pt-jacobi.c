/* Simple Jacobi Iteration Program
 *
 * Phil Nelson, March 1, 2018
 *
 */

#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

/* Globals */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;



/* Data array of size row and col has row+2 rows and
 * col+2 columns.  The extra rows/columns are the fixed
 * edge of the data array.
 *
 */

/* idx macro calculates the correct 2-d based 1-d index
 * of a location (x,y) in an array that has col+2 columns.
 * This is calculated in row major order. 
 */

#define idx(x,y,col)  ((x)*((col)+2) + (y))

/* swap macro -- swap values of a type .*/

#define swap(type,a,b) {type t = a; a = b; b = t; }

/* next_gen calculates the next complete array of values.
 * It uses the 4-point stencil of the average of direct
 * neighbors to the left, right, up and down.
 */

double next_gen ( double *current, double *next, int row, int col )
{
  double diff = 0;
  
  int ix, iy;

  for (ix = 1; ix <= rows ; ix++) {
    /* Calculate all elements in the row. */
    if (ix != args -> rowstart)
      colstart = 0;
    for (iy = colstart; iy <= cols; iy++) {
      /* Calculate new value for location (ix,iy) */
      next[idx(ix, iy, col)] = (current[idx(ix-1, iy, col)]
				+ current[idx(ix+1, iy, col)]
				+ current[idx(ix, iy-1, col)]
				+ current[idx(ix, iy+1, col)]) / 4.0;
      if (pthread_mutex_lock(&mutex))
	printf("error");
      else
	diff = fmax(diff, fabs(current[idx(ix, iy, col)]-next[idx(ix, iy, col)]));
      if (pthread_mutex_unlock(&mutex))
	printf("error");
    }
  }
  pthread_join(tid, NULL)
  return diff;
}

/* Simple initialization routine that initializes row 0 to
 * the value and the rest of the aray to zero.
 */

void initialize (double *ary, int row, int col, double value)
{
  int ix, iy;

  for (iy = 0; iy <= col+1 ; iy++) {
    ary[idx(0,iy,col)] = value;
  }
    
  for (ix = 1; ix <= row+1 ; ix++)
    for (iy = 0; iy <= col+1 ; iy++) {
      ary[idx(ix,iy,col)] = 0;
    }
}

/* print_data prints one of our arrays!
 */

void print_data (double *ary, int row, int col, FILE *outF)
{
  int ix, iy;

  for (ix = 0; ix < row + 2; ix++) {
    for (iy = 0; iy < col + 2; iy++) {
      fprintf (outF, "%.6f ", ary[idx(ix,iy,col)]);
    }
    fprintf (outF, "\n");
  }
}

void usage(char *prog)
{
  fprintf (stderr, "Usage: %s [-n num_threads] [-d] [-r rows] [-c columns] [-t tolerance]"
	           " [-v initial_value] [output_file_name]\n", prog);
  fprintf (stderr, "  tolerance must be greater than zero.\n");
  exit(1);
}

void report (void *args)
{
  printf ("Jacobi done, %d iterations, tolerance %.6f\n", args -> num_iter, args -> tolerance);
  if (args -> outF != stdout) {
    fprintf (args -> outF, "%d %d\n", args -> rows, args -> cols);
  }
  print_data(args -> next, args -> rows, args -> cols, args -> outF);
}

void compute(void *args)
{
  int num_iter = 0;
  int newtolerance = 1;
    /* Compute loop */
  while (newtolerance > args -> tolerance) {
    newtolerance = next_gen(args -> current, args -> next, args -> row, args -> col);
    swap (double *, args -> current, args -> next);
    num_iter++;
  }
  report(void *args);
}

/* Main function
 *
 *  args:  -c columns (default 256)
 *         -d debug output (default off)
 *         -r rows  (default 256)
 *         -t tolerance (default 0.00001)
 *         -v initial value for row 0 (default 10)
 *         -n number of threads (default 8)
 */

int main (int argc, char ** argv)
{
  extern char *optarg;   /* defined by getopt(3) */
  int ch;                /* for use with getopt(3) */

  /* option data */
  double init_value = 10.0;
  static int    rows = 256;
  static int    cols = 256;
  double tolerance = 0.00001;
  int    debug = 0;
  int    num_threads = 8;
  
  while ((ch = getopt(argc, argv, "n:c:dr:t:v:")) != -1) {
    switch (ch) {
    case 'n':  /* number of threads  */
      num_threads = atoi(optarg);
      if (num_threads <= 0)
	usage(argv[0]);
      break;
    case 'c':  /* Columns */
      cols = atoi(optarg);
      if (cols <= 0)
	usage(argv[0]);
      break;

    case 'd':  /* Turn debugging on */
      debug = 1;
      break;

    case 'r':  /* Rows */
      rows = atoi(optarg);
      if (rows <= 0)
	usage(argv[0]);
      break;

    case 't':  /* tolerance greater than zero */
      tolerance = strtod(optarg,NULL);
      if (tolerance <= 0)
	usage(argv[0]);
      break;

    case 'v':  /* initial value */
      init_value = strtod(optarg,NULL);
      break;
      
    case '?': /* help */
    default:
      usage(argv[0]);
    }
  }

  /* Output File */
  FILE *outF = stdout;
  if (optind < argc) {
    if (optind == argc-1) {
      outF = fopen(argv[optind], "w");
      if (outF == NULL) {
	fprintf (stderr, "%s: %s: %s\n", argv[0], argv[optind], strerror(errno));
	exit(1);
      }
    } else {
      usage(argv[0]);
    }
  }

  /* Data arrays */

  double  *ary1 = (double *)malloc(sizeof(double)*((rows+2)*(cols+2)));
  double  *ary2 = (double *)malloc(sizeof(double)*((rows+2)*(cols+2)));

  /* pointers to the arrays */
  double *current = ary1;   /* array to initialize and start computation */
  double *next = ary2;      /* array for new data in computation step */


  /* Initialize */
  initialize (ary1, rows, cols, init_value);
  initialize (ary2, rows, cols, init_value);

  if (debug) {
    printf ("Debug: rows %d, columns %d, initial value %.6f"
	    " tolrance %.6f\n", rows, cols, init_value, tolerance);
    printf ("initial data is:\n");
    print_data (current, rows, cols, stdout);
  }

  
  int indexpthread = ((rows+2) * (cols+2)) / num_thread;
  int start;
  int end;
  struct arg_struct *args;
  
  args.current = current;
  args.next = next;
  args.tolerance = tolerance;
  args.outF = outF;
  
  for (int i = 0; i < num_threads; i++){
    start = i * indexpthread;
    if (i < num_threads - 1)
      end = (i + 1) * indexpthreads;
    else
      end = ((rows + 2) * (cols + 2));
    args.rowend = end/cols;
    args.colend = end%cols;
    args.rowstart = start/cols;
    args.colstart = start%cols;
    pthread_create(&tid[i], NULL, compute, (void *)&args);
  }
  
  //Create threads running next gen and give them each a value 
    /*  for (int j = 0; i < num_threads; i++){
    pthread_create(&tid[i], NULL, compute, (void *)(current, next, &rowstart[i], &colstart[i], tolerance));
    }*/
  
    //int num_iter = compute(current, next, rows, cols, tolerance, num_threads)

  /* Report */

  
  return 0;

}
