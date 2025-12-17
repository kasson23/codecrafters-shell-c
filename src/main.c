#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
  // Flush after every print
  setbuf(stdout, NULL);

  // TODO: Uncomment the code below to pass the first stage
  bool loop_continue = true;
  char user_input[1024];
  printf("$ ");
  while (loop_continue)
  {
    fgets(user_input, sizeof(user_input), stdin);

    user_input[strlen(user_input) - 1] = '\0';
    if (strcmp(user_input, "exit") == 0)
    {
      loop_continue = false;
      break;
    }
    char echoinput[5];
    char output[20];

    // Echo command
    if (strcmp(memcpy(echoinput, user_input, 4), "echo") == 0)
    {
      puts(strcpy(output, user_input + 5));
      printf("$ ");
      continue;
    }

    // Type command
    char typeinput[5];
    char typeoutput[20];

    if (strcmp(memcpy(typeinput, user_input, 4), "type") == 0)
    {
      strcpy(typeoutput, user_input + 5);

      if (strcmp(typeoutput, "echo") == 0)
      {
        printf("echo is a shell builtin\n");
      }
      else if (strcmp(typeoutput, "exit") == 0)
      {
        printf("exit is a shell builtin\n");
      }
      else if (strcmp(typeoutput, "type") == 0)
      {
        printf("type is a shell builtin\n");
      }
      else
      {
        printf("%s: not found\n", typeoutput);
      }
      printf("$ ");

      continue;
    }

    printf("%s: command not found\n", user_input);
    printf("$ ");
  }
  return 0;
}