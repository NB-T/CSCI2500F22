#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 128

int main()
{

  // Open up the text file
  FILE* file = fopen("book-1984.txt", "r");
  if (file == NULL) {
    perror("fopen() failed");
    return EXIT_FAILURE;
  }

  char line[BUF_SIZE];
  while (fgets(line, BUF_SIZE, file) && strcmp(tolower(line), "chapter2") != 0)
  {
     char a = 0;
     for (char* c = line; *c != '\n'; ++c)
     {
          if (isalnum(*c)) 
          {
               ++a;
               printf("%c", *c);
          }
     }
     if (a == 0) continue;
     else printf("[%d alnum chars]\n", a);
  }
  
  fclose(file);

  return EXIT_SUCCESS;
}
