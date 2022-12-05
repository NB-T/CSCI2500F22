#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int MAX_NAME_SIZE = 50; 

void printPermutations(char** permList, int nameSize, int i)
{
   int j;
   if (i == nameSize)
   {
      for (j = 0; j < nameSize - 1; ++j)
      {
         printf("%s, ", permList[j]);
      }
      printf("%s\n", permList[j]);
   }
   
   char tmp[MAX_NAME_SIZE];
   for (j = i; j < nameSize; ++j)
   {
     strcpy(tmp, permList[j]);
     strcpy(permList[j], permList[i]);
     strcpy(permList[i], tmp);
   }

// TODO: Write function to create and output all permutations of the list of names.
void PrintAllPermutations(char **permList, int permSize, char **nameList, int nameSize) {
   printPermutations(nameList, nameSize, 0);
}

int main(void) {
   int size;
   int i = 0;
   char name[MAX_NAME_SIZE];
   scanf("%d", &size);
   char *nameList[size];
   char *permList[size];

   for (i = 0; i < size; ++i) {
      nameList[i] = (char *)malloc(MAX_NAME_SIZE);
      scanf("%s", name);
      strcpy(nameList[i], name);
   }

   PrintAllPermutations(permList, 0, nameList, size);
   
   // Free dynamically allocated memory
   for (i = 0; i < size; ++i) {
      free(nameList[i]);
   }
   
   return 0;
}