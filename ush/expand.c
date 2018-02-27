#include "expand.h"

/* GLOBALS */
extern int gmainargc;
extern char **gmainargv;
extern int shift;
extern int status;

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

      //if its $?
      else if (*(orig+1) == '?'){
	//int istatus = &status;
	orig++;
	
	//if command was a builtin return 0 for success and 1 for failure
	if (status == 0){
	  *new = 0;
	  new++;
	  ix++;
	  orig++;
	}else if (status == 1){
	  *new = 1;
	  new++;
	  ix++;
	  orig++;
	}
	
	//if the command called _exit(2), use the value passed to that function
	if (WIFEXITED(status)){
	  char snum[4];
	  sprintf(snum, "%d", WEXITSTATUS(status));
	  int index = 0;
	  while (ix < newsize && snum[index] != '\0'){
	    *new = snum[index];
	    new++;
	    index++;
	    ix++;
	  }
	  orig++;
	}
	
	//if the command was killed by a signal, use the number 128 plus the signal number
	if (WIFSIGNALED(status)) {
	  char snum[4];
	  sprintf(snum, "%d", (WTERMSIG(status) + 128));
	  int index = 0;
	  while (ix < newsize && snum[index] != '\0'){
	    *new = snum[index];
	    new++;
	    index++;
	    ix++;
	  }
	  orig++;
	}
	
	return 0;
      }
      
      else{
	//$ found but no significant char after
	*new = *orig;
	new++;
	orig++;
	ix++;
      }
      
      //check for * wildcard expansion
    }else if (*(orig+1) == '*'){
      if (*orig == '\\'){
	//just copy the *
	orig++;
	*new = *orig;
	new++;
	orig++;
      }else if (*orig == ' '){
	//leading wildcard
	*new = *orig;
	new++;
	ix++;
	orig++;
	orig++;
	int offset = 1;
	char context [256];
	context[0] = '*';
	while(*(orig+offset) != '\0'){
	  if (offset != 256){
	    if (*(orig+offset) == '/'){
	      perror("context characters include a '/'");
	      return -1;
	    }
	    context[offset] = *(orig+offset);
	    offset++;
	  }else{
	    perror("wildcard: too big");
	    return -1;
	  }
	}
	if (offset > 1) {
	  *context = '\0';
	}
	/*	int i = 0;
	char *newcontext;
	while (context[i] != '\0'){
	  *newcontext = context[i];
	  i++;
	  }8*/
	char buf [256];
	struct dirent *entry;
	//check if null
	char *cwd = getcwd(buf, 256);
	  if (cwd == NULL){
	  perror("getcwd");
	  }
	//check if null
	DIR *dir = opendir(cwd);
	if (dir == NULL){
	  perror("opendir");
	}
	while ((entry = readdir(dir)) != NULL){
	  char *filename = entry->d_name;
	  if (filename[0] != '.'){
	    if (offset > 1) {
	      if (fnmatch(context, filename, 0)== 0){
		*new = ' ';
		new++;
		ix++;
		while (ix < newsize && *filename != '\0'){
		  *new = *filename;
		  ix++;
		  new++;
		  filename++;
		}
	      }
	    }else{
	      *new = ' ';
	      new++;
	      ix++;
	      while (ix < newsize && *filename != '\0'){
		*new = *filename;
	        ix++;
		new++;
		filename++;
	      }
	    }
	  }
	}
	closedir(dir);
      }
    }else{
      //no $ | * so simple copy
      *new = *orig;
      new++;
      orig++;
      ix++;
    }
  } 
  *new = '\0';
  return 1;
}

