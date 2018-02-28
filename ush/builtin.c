#include "builtin.h"

/* Globals */
extern int gmainargc;
extern int status;
int shift = 0;


const char *sexit     = "exit";
const char *senvset   = "envset";
const char *senvunset = "envunset";
const char *scd       = "cd";
const char *sshift    = "shift";
const char *sunshift  = "unshift";
const char *ssstat    = "sstat";

void print_perms(int mode);

void do_pipe (int fd[], char *buffer);

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
	  status = 1;
	  exit(status);
	}else{
	  status = atoi(argv[1]);
	  exit(status);
	}
      }

      break;
      
    case 's':
      //envset
      if (strcmp(command, senvset) == 0){
	if (setenv(argv[1], argv[2], 1)){
	  perror("setenv: ");
	  return 1;
	}
	status = 0;
	return 0;
      }
      break;
    case 'u':
      //envunset
      if (strcmp(command, senvunset) == 0){
	if (unsetenv(argv[1])){
	  perror("unsetenv: ");
	  return 1;
	}
	return 0;
      }
      break;
    }
  case 'c':
    //cd
    if (strcmp(command, scd) == 0){
      if (chdir(argv[1]) < 0){
	perror("cd: ");
	return 1;
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
	    printf("shift: shift too large\n");
	    return 1;
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
	    
	    printf("%s ", argv[c]); //print filename
	    
	    if ((pwd = getpwuid(statbuf.st_uid)) != NULL){
	      printf("%s ", pwd->pw_name); //print user name
	    }else{
	      printf("%ld ", (long)statbuf.st_uid); //print uid
	    }
	    
	    if ((grp = getgrgid(statbuf.st_gid)) != NULL){
	      printf("%s ", grp->gr_name); //print group name
	    }else{
	      printf("%ld ", (long)statbuf.st_gid); //print gid
	    }
	    
	    int mode = statbuf.st_mode;
	    print_perms(mode); //print permission list
	    
	    printf("%ld ", (long)statbuf.st_nlink); //print number of links
	    
	    printf("%lld ", (long long)statbuf.st_size); //print size in bytes
	    
	    printf("%s", asctime(localtime(&statbuf.st_mtime))); //print time localtime asctime
	    
	    c++;
	  }else{
	    perror("stat: ");
	    return 1;
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
	  printf("unshift: shift too large\n");
	  return 1;
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

void print_perms(int mode){
  printf("-");
  /* Check owner permissions */
  if ((mode & S_IRUSR) && (mode & S_IREAD)){
    printf("r");
  }
  else{
    printf("-");
  }
  if ((mode & S_IWUSR) && (mode & S_IWRITE)) {
    printf("w");
  }
  else{
    printf("-");
  }
  if ((mode & S_IXUSR) && (mode & S_IEXEC)){
    printf("x");
  }
  else{
    printf("-");
  }
  /* Check group permissions */
  if ((mode & S_IRGRP) && (mode & S_IREAD)){
    printf("r");
  }
  else{
    printf("-");
  }
  if ((mode & S_IWGRP) && (mode & S_IWRITE)){
    printf("w");
  }
  else{
    printf("-");
  }
  if ((mode & S_IXGRP) && (mode & S_IEXEC)){
    printf("x");
  }
  else{
    printf("-");
  }
  /* check other user permissions */
  if ((mode & S_IROTH) && (mode & S_IREAD)){
    printf("r");
  }
  else{
    printf("-");
  }
  if ((mode & S_IWOTH) && (mode & S_IWRITE)){
    printf("w");
  }
  else{
    printf("-");
  }
  if ((mode & S_IXOTH) && (mode & S_IEXEC)){
    printf("x ");
  }
  else{
    printf("- ");
  }
}


void do_pipe(int fd[], char *buffer){
  //create pipe
  pipe(fd);

  //write to stdin
  write(fd[1], buffer, strlen(buffer));
  
  //read from stdout
  int n;
  while ((n = read(fd[0], buffer, strlen(buffer))) > 0);
  close(fd[0]);
  close(fd[1]);
  return;
  }
