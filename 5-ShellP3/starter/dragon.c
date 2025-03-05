#include <stdio.h>

// EXTRA CREDIT - print the drexel dragon from the readme.md
extern void print_dragon(){
  FILE *fptr = NULL;

  fptr = fopen("dragon.txt", "r");
  printf("\n");
  
  char line[256];

  while(fgets(line, 256, fptr) != NULL) {
      printf("%s", line);
  }
  printf("\n");
  fclose(fptr);
}
