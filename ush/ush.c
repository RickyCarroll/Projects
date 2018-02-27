/* CS 352 -- Micro Shell!  
 *
 *   Sept 21, 2000,  Phil Nelson
 *   Modified April 8, 2001 
 *   Modified January 6, 2003
 *   Modified January 8, 2017
 *
 */

#include "ush.h"

/* Constants */ 

#define LINELEN 1024

/* Prototypes */

void processline (char *line);
char** arg_parse(char *line, int *argcptr);
int arg_count(char *line);

/* Globals */

int gmainargc = 0;
char **gmainargv = NULL;
int status;

/* Shell main */

int
main (int mainargc, char **mainargv)
{
  char   buffer [LINELEN];
  int    len;
  int    cpound;
  char   *mode = "r";
  FILE *file;
  gmainargc = mainargc;
  gmainargv = mainargv;
  
  if (mainargc > 1){
    file = fopen(mainargv[1], mode);
    if (file == NULL){
      perror("Could not open file");
      exit(127);
    }
  }
  
  while (1) {
    
    if (mainargc == 1){
      /* prompt and get line */
      fprintf (stderr, "%% ");
      if (fgets (buffer, LINELEN, stdin) != buffer)
	break;
    }else if (feof(file)){
      fclose(file);
      exit(0);
    }else{
      if (fgets(buffer, LINELEN, file) != buffer){
	break;
      }
    }
    
    /* Get rid of \n at end of buffer. */
    len = strlen(buffer);
    if (buffer[len-1] == '\n')
      buffer[len-1] = 0;
    
    /* Get rid of comments */
    //checks all but the first element
    if (buffer[0] != '#'){
      cpound = 1;
      while (cpound < len){
	if (buffer[cpound] == '#'){
	  if (buffer[cpound-1] != '$'){
	    buffer[cpound] = '\0';
	    break;
	  }
	}
	cpound++;
      }
      /* Run it ... */
      processline(buffer);
    }
    
  }
  
  if (!feof(stdin))
    perror ("read");
  
  return 0;		/* Also known as exit (0); */
}


void processline (char *line)
{
  pid_t  cpid;
  int    err = 0;
  char   newline [LINELEN];
  
  err = rc_expand(line, newline, LINELEN);
  if (err < 0){
    perror("expand");
    return;
  }
  
  int argc = arg_count(newline);
  if (argc == 0){
    return;
  }
  
  char **argv = arg_parse(newline, &argc);
  
  err = rc_builtin(argv, &argc);
  if (err == 0){
    //successful builtin command
    return;
    
  }else{
    /* Start a new process to do the job. */
    cpid = fork();
    if (cpid < 0) {
      /* Fork wasn't successful */
      perror ("fork");
      return;
    }
    
    /* Check for who we are! */
    if (cpid == 0) {
      /* We are the child! */
      /* execvp returned, wasn't successful */
      err = execvp(argv[0], argv);
      printf("exec: error\n");
      free(argv);
      fclose(stdin);  // avoid a linux stdio bug
      exit (127);
    }else{
      free(argv);
    }
    
    /* Have the parent wait for child to complete */
    if (wait (&status) < 0) {
      /* Wait wasn't successful */
      perror ("wait");
    }
  }
}

int arg_count(char *line){
  char *cpline = line;
  int argc = 0;
  //loop until the end to find number of args
  while (*cpline != '\0'){
    //loop until first non-space char
    while (*cpline == ' ' && *cpline != '\0'){
      cpline++;
    }
    //loop until the end of arg
    while (*cpline != ' ' && *cpline != '\0'){
      cpline++;
    }
    //increment argcounter
    argc++;
  }
  return argc;
}

char** arg_parse(char *line, int *argcptr){
  char **argv = (char **) malloc (sizeof(char *) * ((*argcptr)+1));
  int count = 0;
  int ix = 0;
  while (line[ix] != '\0'){
    //loop until first non-space char
    while (line[ix] == ' ' && line[ix] != '\0'){
      ix++;
    }
    if (count < *argcptr){
      argv[count] = &line[ix];
      count++;
    }
    //loop until the end of arg
    while (line[ix] != ' ' && line[ix] != '\0'){
      ix++;
    }
    if (line[ix] == '\0'){
      return argv;
    }
    line[ix] = '\0';
    ix++;
  }
  return argv;
}

