#include "defn.h"

int rc_copy (char *from, int fix, char *to, int tix){
  while (from[fix] != '\0'){
    to[tix] = from[fix];
    tix++;
    fix++;
  }
  return 0;
}

int expand (char *orig, char *new, int newsize){
  //loop through orig until find EOS
  while (*orig != '\0'){
    //if find expand char
    if (*orig == '$'){
      orig++;
      //look for following command char
      
      //If its '$$'
      if (*orig == '$'){
	
      }
      //If its '${'
      if (*orig == '{'){
	orig++;
	char* start = orig;
	//loop to find '}' or EOS
	while (*orig != '}' && *orig != '\0'){
	  orig++;
	}
	*orig = '\0';
	char *env = getenv(start);
	if (env == NULL){
	  printf("expand: ");
	  return -1;
	}
	int ix = 0;
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
  return 1;
}
