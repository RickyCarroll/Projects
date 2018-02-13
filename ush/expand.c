#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include "defn.h"

/* GLOBALS */
extern int gmainargc;
extern char **gmainargv;
extern int shift;

int rc_expand (char *orig, char *new, int newsize){
  int ix = 0;
  //loop through orig until find EOS
  while (*orig != '\0'){
    //if find expand char
    if (*orig == '$'){
      
      //look for following command char
      
      //If its '$$'
      if (*(orig+1) == '$'){
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
      else if (*(orig+1) == '{'){
	orig++;
	orig++;
	char* start = orig;
	//loop to find '}' or EOS
	while (*orig != '}' && *orig != '\0'){
	  orig++;
	}
	if (*orig == '\0'){
	  perror("expand: missing brace");
	  return -1;
	}
	*orig = '\0';
	char *env = getenv(start);
	if (env == NULL){
	  perror("expand: env not found");
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
      
      //if its $#
      else if (*(orig+1) == '#'){
	orig++;
	char snum[5];
	sprintf(snum, "%d", gmainargc-1-shift);
	int index = 0;
       	while (ix < newsize && snum[index] != '\0'){
	  *new = snum[index];
	  new++;
	  index++;
	  ix++;
	}
	orig++;
      }

      //if its $n
      else if (isdigit(*(orig+1)) > 0){
	orig++;
	int digit = *orig - '0';
	while(isdigit(*(orig+1)) > 0){
	  orig++;
	  digit = (digit*10 + (*orig - '0'));
	}

        if (digit + 1 + shift < gmainargc){
	  char *arg = gmainargv[digit+1+shift];
	  while (ix < newsize && *arg != '\0'){
	    *new = *arg;
	    new++;
	    arg++;
	    ix++;
	  }
	  orig++;
	}else{
	  orig++;
	}
      }
      else{
	//$ found but no significant char after
	*new = *orig;
	new++;
	orig++;
      }
    }else{
      //no $, simple copy
      *new = *orig;
      new++;
      orig++;
    }
  }
  *new = '\0';
  return 1;
}
