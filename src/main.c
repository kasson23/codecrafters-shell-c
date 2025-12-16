#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// helpful commands
/*
' gcc -Wall -Wextra main.c -o main.exe '
' '
*/
int main()
{

  // Create an integer variable that will store the number we get from the user
  char userInput[100];
  char errorMsg[] = ": command not found";
  char output[200];

  fgets(userInput, sizeof(userInput), stdin);

  // Remove newline from fgets
  userInput[strcspn(userInput, "\n")] = 0;

  strcpy(output, userInput);
  strcat(output, errorMsg);

  printf("%s\n", output);

  return 0;
}
