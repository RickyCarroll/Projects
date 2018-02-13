#include "defn.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

/* Globals */
extern int gmainargc;
int shift = 0;

const char *sexit =     "exit";
const char *senvset =   "envset";
const char *senvunset = "envunset";
const char *scd =       "cd";
const char *sshift =    "shift";
const char *sunshift =  "unshift";


int rc_builtin(char **argv, int *argc){
  char *command = argv[0];
  char first = argv[0][0];
  char fourth = argv[0][3];

  switch(first){
  case 'e':
    switch(fourth){
    case 't':
      //exit
      if (strcmp(command, sexit) == 0){
        if (*argc == 1){
	  exit(1);
	}else{
	  int status = atoi(argv[1]);
	  exit(status);
	}
      }

      break;
      
    case 's':
      //envset
      if (strcmp(command, senvset) == 0){
	setenv(argv[1], argv[2], 1);
	return 0;
      }
      break;
    case 'u':
      //envunset
      if (strcmp(command, senvunset) == 0){
	unsetenv(argv[1]);
	return 0;
      }
      break;
    }
  case 'c':
    //cd
    if (strcmp(command, scd) == 0){
      if (chdir(argv[1]) < 0){
	printf("cd: %s\n",strerror(errno));
      }
      return 0;
    }
    break;

  case 's':
    //shift
    if (strcmp(command, sshift) == 0){
      if (*argc == 1){
	shift++;
      }else {
	int temp = atoi(argv[1]);
	if (shift + temp > gmainargc){
	  perror("shift: shift too large");
	}else{
	  shift += temp;
	}
      }
    return 0;
    }
    break;
    
  case 'u':
    //unshift
    if (strcmp(command, sunshift) == 0){
      if (*argc == 1){
	shift = 0;
      }else {
	int temp = atoi(argv[1]);
	if (shift - temp < 0){
	  perror("unshift: shift too large");
	}else{
	  shift -= temp;
	}
      }
      return 0;
    }
    break;
  }
  return -1;
}
