#include <stdio.h>
int main (int argc, char** argv){
  int ix;
  for (ix=0; ix<argc; ix++){
    printf("Arg %d: '%s'\n", ix, argv[ix]);
  }
  return 0;
}
