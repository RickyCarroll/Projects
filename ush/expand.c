#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "defn.h"

int rc_copy (char *from, int fix, char *to, int tix){
  while (from[fix] != '\0'){
    to[tix] = from[fix];
    tix++;
    fix++;
  }
  return 0;
}

int rc_expand (char *orig, char *new, int newsize){
  int ix = 0;
  //loop through orig until find EOS
  while (*orig != '\0'){
    //if find expand char
    if (*orig == '$'){
      orig++;
      
      //look for following command char
      
      //If its '$$'
      if (*orig == '$'){
	char spid[5];
	pid_t pid = getpid();
	sprintf(spid, "%d", pid);
	int index = 0;
       	while (ix < newsize && spid[index] != '\0'){
	  *new = spid[index];
	  new++;
	  index++;
	  ix++;
	}
      }
      //If its '${'
      else if (*orig == '{'){
	orig++;
	char* start = orig;
	//loop to find '}' or EOS
	while (*orig != '}' && *orig != '\0'){
	  orig++;
	}
	if (*orig == '\0'){
	  printf("expand: missing brace\n");
	  return -1;
	}
	*orig = '\0';
	char *env = getenv(start);
	if (env == NULL){
	  printf("expand: env not found\n");
	  return -1;
	}
	//copies what is returned from getenv into new
	while (ix < newsize && *env != '\0'){
	  *new = *env;
	  new++;
	  env++;
	  ix++;
	}

	*orig = '}';
	orig++;
      }
      
    }else{
      //no special character, simple copy
      *new = *orig;
      new++;
      orig++;
    }
  }
  *new = '\0';
  return 1;
}
