#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void find_path_of_type_cmd(char paths[], char executable_name[])
{
  char *paths_copy = strdup(paths);
  char *remaining = paths_copy;
  bool found = false;

  while (!found && remaining != NULL)
  {
    char *semicolon = strchr(remaining, ';');

    // Extract current directory path
    char path[260];
    if (semicolon == NULL)
    {
      strcpy(path, remaining);
      remaining = NULL;
    }
    else
    {
      strncpy(path, remaining, semicolon - remaining);
      path[semicolon - remaining] = '\0';
      remaining = semicolon + 1;
    }

    // Try to open directory
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
      continue;
    }

    struct dirent *de;
    while ((de = readdir(dir)) != NULL)
    {
      // Check if filename matches executable name
      if (strcmp(de->d_name, executable_name) == 0)
      {
        // Build full path
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s\\%s", path, executable_name);

        // Check if file has execute permissions
        struct stat st;
        if (stat(full_path, &st) == 0 && (st.st_mode & S_IXUSR))
        {
          printf("%s is %s\n", executable_name, full_path);
          found = true;
          break;
        }
      }
    }
    closedir(dir);
  }

  if (!found)
  {
    printf("%s: not found\n", executable_name);
  }

  free(paths_copy);
}

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
    char executable_name[20];
    if (strcmp(memcpy(typeinput, user_input, 4), "type") == 0)
    {
      strcpy(executable_name, user_input + 5);

      if (strcmp(executable_name, "echo") == 0 || strcmp(executable_name, "exit") == 0 || strcmp(executable_name, "type") == 0)
      {
        printf("%s is a shell builtin\n", executable_name);
        printf("$ ");
        continue;
      }

      char *paths = getenv("PATH");

      find_path_of_type_cmd(paths, executable_name);

      printf("$ ");
      continue;
    }

    printf("%s: command not found\n", user_input);
    printf("$ ");
  }
  return 0;
}