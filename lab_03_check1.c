#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARR_SIZE 1024

int main()
{
 
  FILE* file = fopen("lab03-data.dat", "r");  // man fopen 
  if (file == NULL) {
    perror("fopen() failed"); // use this if errno is set (see man page)
    return EXIT_FAILURE;
  }

  int* data = calloc(ARR_SIZE, sizeof(int));
  int size = fread(data, sizeof(int), ARR_SIZE, file);
  
  for (int i = 0; i < size; ++i)
  {
       printf("Data point #%3d: %d\n", i, data[i]);
  }
  
  fclose(file);

  return EXIT_SUCCESS;
}

