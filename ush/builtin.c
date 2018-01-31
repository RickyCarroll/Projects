#include "defn.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int rc_isbuiltin(char **argv, int *argc){
  /*  char *sexit = "exit";
  char *senvset = "envset";
  char *senvunset = "envunset";
  char *scd = "cd";*/
    
  char command = argv[0][0];
  char fourth = argv[0][3];

  switch(command){
  case 'e':
    switch(fourth){
    case 't':
      //exit
      if (*argc == 1){
	exit(0);
      }
      else {
	int status = atoi(argv[1]);
	exit(status);
      }
      
    case 's':
      //envset
      setenv(argv[1], argv[2], 1);
      return 0;
      
    case 'u':
      //envunset
      unsetenv(argv[1]);
      return 0;
      
    }
    break;
  case 'c':
    //cd
    if (chdir(argv[1]) < 0){
      printf("cd: %s\n",strerror(errno));
    }
    return 0;
  }
  
  return -1;
}
