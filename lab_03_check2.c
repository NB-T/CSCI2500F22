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

  unsigned long* data = calloc(ARR_SIZE, sizeof(unsigned long));
  int size = fread(data, sizeof(unsigned long), ARR_SIZE, file);
  
  for (int i = 0; i < size; ++i)
  {
       printf("Data point #%3d: %ld\n", i, data[i]);
  }
  
  fclose(file);

  return EXIT_SUCCESS;
}
