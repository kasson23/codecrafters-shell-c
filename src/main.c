#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// helpful commands
/*
' gcc -Wall -Wextra main.c -o main.exe '
' .\main.exe '
*/
int main()
{

  while (true)
  {
    printf("$ ");

    // Wait for user input
    char input[100];
    fgets(input, 100, stdin);

    // Remove the trailing newline
    input[strlen(input) - 1] = '\0';
    printf("%s: command not found\n", input);
  }

  return 0;
}
