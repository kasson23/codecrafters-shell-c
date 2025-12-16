#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // string methods
#include <stdbool.h> // booleans
// helpful commands
/*
' gcc -Wall -Wextra main.c -o main.exe '
' .\main.exe '
*/
int main()
{

  bool run = true;

  while (run)
  {
    printf("$ ");

    // Wait for user input
    char input[100];
    fgets(input, 100, stdin);

    // Remove the trailing newline
    input[strlen(input) - 1] = '\0';

    int equals = strcmp(input, "exit");
    if (equals == 0)
    {
      run = false;
      return 0;
    }

    printf("%s: command not found\n", input);
  }

  return 0;
}
