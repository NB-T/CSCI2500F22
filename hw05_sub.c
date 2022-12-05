#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

void debug()
{
     printf("still good\n");
}

int mod(int i)
{
     return i % 7;
}

int mod10(int i)
{
     return i % 10;
}

// 0 addition, 1 subtraction, 2 multiplication, 3 division, 4 modulus
int op(char *current)
{
     if (strcmp(current, "+") == 0) return 0;
     if (strcmp(current, "-") == 0) return 1;
     if (strcmp(current, "*") == 0) return 2;
     if (strcmp(current, "/") == 0) return 3;
     if (strcmp(current, "%") == 0) return 4;
     return -1;
}

bool isNumber(char *current)
{
     int start;
     if (current[0] == '-') start = 1;
     else start = 0;
     for (int i = start; i < (int) strlen(current); ++i)
     {
          if (!isdigit(current[i])) return false;
     }
     return true;
}

// FOR SAVED REGISTERS
// update reg to match its already used register, or else use a new register
void setSRegister(char **reg, char *variable, bool *register_used, char *variable_at_register, char **sregisters)
{
     // if variable is already assigned to a register, update reg
     for (int i = 0; i < 8; ++i)
     {
          if (variable_at_register[i] == *variable)
          {
               *reg = sregisters[i];
               return;
          }
     }
     // if reg was not updated
     int i = 0;
     while (register_used[i])
          ++i;
     // set unused register to used
     // set next one to unused (for wrap-around)
     // update reg
     register_used[mod(i)] = true;
     variable_at_register[mod(i)] = *variable;
     register_used[mod(i + 1)] = false;
     *reg = sregisters[mod(i)];

     return;
}

// FOR T REGISTERS
// update reg to match its already used register, or else use a new register
void setTRegister(char **reg, char *variable, bool *register_used, char *variable_at_register, char **tregisters)
{
     // if variable is already assigned to a register, update reg
     for (int i = 0; i < 10; ++i)
     {
          if (variable_at_register[i] == *variable)
          {
               *reg = tregisters[i];
               return;
          }
     }
     // if reg was not updated
     int i = 0;
     while (register_used[i])
          ++i;
     // set unused register to used
     // set next one to unused (for wrap-around)
     // update reg
     register_used[mod10(i)] = true;
     variable_at_register[mod10(i)] = *variable;
     register_used[mod10(i + 1)] = false;
     *reg = tregisters[mod10(i)];

     return;
}

int main(int argc, char *argv[])
{
     if (argc != 2)
     {
          printf("Usage: ./hw05.out filename\n");
          return EXIT_FAILURE;
     }
     char filename[128];
     sscanf(argv[1], "%s", filename);
     FILE *file = fopen(filename, "r");

     // if register_used[i % 10], then $t{i % 10} is used
     // when register_used[i % 10] is switched from false to true:
     //      register[(i + 1) % 10] is set to false
     bool register_used[10];
     for (int i = 0; i < 10; ++i)
          register_used[i] = false;

     // variable_at_register[i] is the name of the variable at $t{i}
     char variable_at_register[10];

     for (int i = 0; i < 10; ++i)
          variable_at_register[i] = ' ';

     char *sregisters[] = {"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9"};

     char *tregisters[] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9"};

     // SOME PRINT DEBUGGING
     // char* reg = "";
     // setSRegister(reg, current, register_used, variable_at_register, registers);
     // printf("\n");
     // current = "b";
     // setSRegister(reg, current, register_used, variable_at_register, registers);
     // for (int i = 0; i < 10; ++i)
     // {
     //      printf("%d %c %d %s\n", register_used[i], variable_at_register[i], register_used[mod(i + 1)], registers[i]);
     // }

     char line[512];
     // read lines
     while (fgets(line, 512, file))
     {
          printf("# %s", line);

          char** tokens = NULL;
          char* current = strtok(line, " ;");
          tokens = (char**) realloc(tokens, sizeof(*tokens));
          int i = 0;
          while (current)
          {
               // printf("%s\n", current);
               tokens = (char**) realloc(tokens, (i + 1)*sizeof(*tokens));
               tokens[i] = (char*) malloc(2*sizeof(current));
               strcpy(tokens[i], current);
               if (i % 2 == 0 && !isNumber(tokens[i])) 
               {
                    char* reg = (char*) malloc(2*sizeof(char));
                    setSRegister(&reg, tokens[i], register_used, variable_at_register, sregisters);
                    strcpy(tokens[i], reg);
               }
               ++i;
               current = strtok(NULL, " ;");
          }
          --i;

          /* printf("tokens: ");
          for (int j = 0; j < i; ++j) printf("%s ", tokens[j]);
          printf("\n"); */
          
          // set up operands
          // constants or registers for each MIPS line
          char* operand1 = (char*) malloc(8*sizeof(char));
          char* operand2 = (char*) malloc(8*sizeof(char));
          char* operand3 = (char*) malloc(8*sizeof(char));

          int operation;
          
          // simple assignment
          if (i == 3)
          {
               strcpy(operand1, tokens[0]); // LHS
               strcpy(operand2, tokens[2]); // RHS
               if (isNumber(operand2)) printf("li $%s,%s\n", operand1, operand2);
               else 
               {
                    setTRegister(&operand2, operand2, register_used, variable_at_register, tregisters);
                    printf("move $%s,$%s\n", operand1, operand2);
               }
          }

          else
          {
               operation = op(tokens[3]);
               // addition/subtraction
               if (operation == 0 || operation == 1)
               {
                    strcpy(operand1, tokens[0]); // LHS
                    // when we hit the last operand, operand1 is sregister from LHS
                    // else, operand1 should be tregister
                    for (int j = 2; j < i - 2; j += 2)
                    {
                         operation = op(tokens[j + 1]);
                         strcpy(operand2, tokens[j]);
                         strcpy(operand3, tokens[j + 2]);
                         bool a = isNumber(operand3);
                         if (j != i - 3)
                         {
                              if (a) 
                              {
                                   if (operation == 0) printf("addi $t0,$%s,%s\n", operand2, operand3);
                                   else printf("addi $t0,$%s,-%s\n", operand2, operand3);
                              }
                              else printf("add $t0,$%s,$%s\n", operand2, operand3);
                         }
                         else
                         {
                              if (a) 
                              {
                                   if (operation == 0) printf("addi $%s,$t0,%s\n", operand1, operand3);
                                   else printf("addi $%s,$t0,-%s\n", operand1, operand3);
                              }
                              else printf("add $%s,$t0,$%s\n", operand1, operand3);
                         }
                    }
               }

               // multiplication/division/modulus
               else
               {

               }

          }
          
          for (int j = 0; j < i; ++j)
          {
               free(tokens[j]);
          }
          free(tokens);
          // free(operand1);
          // free(operand2);
          // free(operand3);
     }
     fclose(file); 
     return EXIT_SUCCESS;
}