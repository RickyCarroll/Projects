/* CS 352 -- Miro Shell!  
 *
 *   Sept 21, 2000,  Phil Nelson
 *   Modified April 8, 2001 
 *   Modified January 6, 2003
 *   Modified January 8, 2017
 *
 */

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "defn.h"

/* Constants */ 

#define LINELEN 1024

/* Prototypes */

void processline (char *line);

/* My functions */

char** arg_parse(char *line, int *argcptr);
int arg_count(char *line);

/* Shell main */

int
main (void)
{
    char   buffer [LINELEN];
    int    len;

    while (1) {

        /* prompt and get line */
	fprintf (stderr, "%% ");
	if (fgets (buffer, LINELEN, stdin) != buffer)
	  break;

        /* Get rid of \n at end of buffer. */
	len = strlen(buffer);
	if (buffer[len-1] == '\n')
	    buffer[len-1] = 0;

	/* Run it ... */
      	processline(buffer);

    }

    if (!feof(stdin))
        perror ("read");

    return 0;		/* Also known as exit (0); */
}


void processline (char *line)
{
    pid_t  cpid;
    int    status;
    int    err = 0;
    char   newline [LINELEN];
    int argc = arg_count(line);
    if (argc == 0){
      return;
    }
    err = expand(line, newline, LINELEN);
    if (err < 0){
      perror("expand");
      return;
    }
    char **argv;
    if (err != 1){
      argv = arg_parse(line, &argc);
    } else{
      argv = arg_parse(newline, &argc);
    }
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
      execvp(argv[0], argv);
      perror ("exec");
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
  char ** argv = {NULL};
  argv = (char **) malloc (sizeof(char) * *argcptr);
  char *cpline = line;
  size_t size = strlen(line);
  int count = 0;
  int ix = 0;
  //duplicate line to cpline
  while (ix < size){
    //loop until first non-space char
    while (cpline[ix] == ' ' && cpline[ix] != '\0'){
      ix++;
    }
    if (count < *argcptr){
      argv[count] = &cpline[ix];
      count++;
    }
    //loop until the end of arg
    while (cpline[ix] != ' ' && cpline[ix] != '\0'){
      ix++;
    }
    cpline[ix] = '\0';
    ix++;
  }
  return argv;
}

