#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// add, addi, sub, and, andi, or, ori, lw, and sw

int main(int argc, char *argv[])
{
     if (argc != 2)
     {
          printf("Usage: ./hw05.out filename\n");
          exit(EXIT_FAILURE);
     }
     char filename[128];
     sscanf(argv[1], "%s", filename);
     FILE *file = fopen(filename, "r");

     if (file == NULL)
     {
          printf("failed to open file.\n");
          exit(EXIT_FAILURE);
     }
     printf("START OF SIMULATION\n\n");
     char instructions[5][256];
     char line[256];
     int i = 0;
     while (fgets(line, 256, file))
     {
          line[strcspn(line, "\r\n")] = 0;
          strcpy(instructions[i], line);
          ++i;
     }
     
     int num_lines = i;
     
     for (; i < 5; ++i)
     {
          strcpy(instructions[i], "");
     }

     int num_steps;
     if (num_lines <= 5) num_steps = num_lines + 4;
     else num_steps = 9;
     int cycle;
     for (cycle = 0; cycle < num_steps; ++cycle)
     {
          printf("CPU Cycles ===>");
          for (i = 1; i <= 9; ++i) printf("\t%d", i);
          printf("\n");
          for (i = 0; i < num_lines; ++i)
          {
               printf("%s ", instructions[i]);
               for (int j = 0; j < 9; ++j) printf("\t.");
               printf("\n");
          }
          printf("\n");
     }
     printf("END OF SIMULATION");
     return EXIT_SUCCESS;
}