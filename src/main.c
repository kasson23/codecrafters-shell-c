#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // string methods
#include <stdbool.h> // booleans
// helpful commands
/*
' gcc -Wall -Wextra main.c -o main.exe '
' gcc -g -Wall -Wextra main.c -o main.exe ' - needs 'g' for debugging
' .\main.exe '
*/

void remove_up_to_index(char *str, int index)
{
  int len = strlen(str);

  // Validate index so we don't read or write out of bounds
  if (index < 0 || index >= len)
  {
    return;
  }

  /*
   * memmove(destination, source, number_of_bytes)
   *
   * dest = str
   *   We want the remaining part of the string to start at index 0
   *
   * src = str + index
   *   This points to the character we want to keep
   *
   * len - index + 1
   *   Number of bytes to move:
   *     - all remaining characters
   *     - PLUS the null terminator ('\0')
   *
   * memmove is used instead of memcpy because
   * source and destination overlap in memory.
   */
  memmove(str, str + index + 1, len - index + 1);
}

int main()
{

  bool run = true;

  while (run)
  {
    char input[100];
    char cmd[100];

    printf("$ ");
    fflush(stdout);

    // Wait for user input
    fgets(input, 100, stdin);

    int length = strlen(input);

    // Remove the trailing newline
    input[length - 1] = '\0';

    // need to find where first ' ' is and this will be command - returns pointer
    char *ptr = strchr(input, ' ');

    if (ptr != NULL)
    {
      int index = (int)(ptr - input);

      strncpy(cmd, input, index);
      cmd[index] = '\0';

      remove_up_to_index(input, index);
    }

    int exit = strcmp(cmd, "exit");
    if (exit == 0)
    {
      run = false;
      return 0;
    }

    int echo = strcmp(cmd, "echo");
    if (echo == 0)
    {
      printf("%s\n", input);
      printf("$ ");
      continue;
    }

    printf("%s: command not found\n", input);
  }

  return 0;
}
