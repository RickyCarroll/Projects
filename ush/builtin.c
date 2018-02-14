#include "defn.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

/* Globals */
extern int gmainargc;
int shift = 0;

const char *sexit     = "exit";
const char *senvset   = "envset";
const char *senvunset = "envunset";
const char *scd       = "cd";
const char *sshift    = "shift";
const char *sunshift  = "unshift";
const char *ssstat    = "sstat";


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
    switch (fourth){
      
    case 'f':
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
      
    case 'a':
      //sstat
      if (strcmp(command, ssstat) == 0){
	struct stat statbuf;
	struct group *grp = NULL;
	struct passwd *pwd = NULL;
	int c = 1;
	//loop through all args (filenames)
	while (c < *argc){
	  if (stat(argv[c], &statbuf) == 0){
	    fprintf(stderr, "%s ", argv[c]); //print filename
	    if ((pwd = getpwuid(statbuf.st_uid)) != NULL){
	      fprintf(stderr, "%s ", pwd->pw_name); //print user name
	    }else{
	      fprintf(stderr, "%ld ", (long)statbuf.st_uid); //print uid
	    }
	    if ((grp = getgrgid(statbuf.st_gid)) != NULL){
	      fprintf(stderr, "%s ", grp->gr_name); //print group name
	    }else{
	      fprintf(stderr, "%ld ", (long)statbuf.st_gid); //print gid
	    }
	    fprintf(stderr, "%lo ", (unsigned long)statbuf.st_mode); //print permission list **
	    fprintf(stderr, "%ld ", (long)statbuf.st_nlink); //print number of links
	    fprintf(stderr, "%lld ", (long long)statbuf.st_size); //print size in bytes
	    fprintf(stderr, "%s ", asctime(localtime(&statbuf.st_mtime))); //print time localtime asctime
	    c++;
	  }else{
	    perror("stat");
	    return 0;
	  }
	}
	return 0;
      }
      break;
    }
    
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
