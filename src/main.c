#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// These headers are only available on Unix/Linux systems
#ifdef __unix__
#include <sys/wait.h>
#endif

// Print where an executable is found (for the "type" command)
// This function searches through PATH directories to find an executable
void find_path_of_type_cmd(char paths[], char executable_name[])
{
  // strdup() makes a copy of the string on the heap (dynamic memory)
  // We do this because strtok() would modify the original PATH string,
  // which we don't want to do since getenv() returns a pointer to the environment
  char *paths_copy = strdup(paths);
  char *remaining = paths_copy;
  bool found = false;

  while (!found && remaining != NULL)
  {
    // strchr() finds the first occurrence of ':' (PATH separator on Linux)
    // Returns NULL if not found (meaning we've reached the last directory)
    char *delimiter = strchr(remaining, ':');

    // Extract current directory path between colons
    char path[260];
    if (delimiter == NULL)
    {
      // No more colons, copy what's left as the final directory
      strcpy(path, remaining);
      remaining = NULL; // Signal we're done
    }
    else
    {
      // Copy from current position up to (but not including) the colon
      // (delimiter - remaining) calculates the number of characters to copy
      strncpy(path, remaining, delimiter - remaining);
      path[delimiter - remaining] = '\0'; // Add null terminator
      remaining = delimiter + 1;          // Move to next directory
    }

    // Try to open directory
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
      continue; // Skip directories that can't be opened
    }

    struct dirent *de; // dirent = directory entry (a file in the directory)
    while ((de = readdir(dir)) != NULL)
    {
      // Check if filename matches executable name (on Linux, no extension needed)
      if (strcmp(de->d_name, executable_name) == 0)
      {
        // Build full path
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, de->d_name);

        // Check if file exists, is a regular file, and has execute permissions
        struct stat st; // stat = file statistics (size, permissions, etc)
        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR))
        {
          printf("%s is %s\n", executable_name, full_path);
          found = true;
          break;
        }
      }
      else
      {
        // Also check with .exe extension for Windows compatibility
        char *last_dot = strrchr(de->d_name, '.');
        if (last_dot != NULL && strcmp(last_dot + 1, "exe") == 0)
        {
          // Extract filename without extension
          char filename[last_dot - de->d_name + 1];
          strncpy(filename, de->d_name, last_dot - de->d_name);
          filename[last_dot - de->d_name] = '\0';

          if (strcmp(filename, executable_name) == 0)
          {
            // Build full path (print without .exe extension)
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, executable_name);

            // Check if file exists, is a regular file, and has execute permissions
            struct stat st;
            if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR))
            {
              printf("%s is %s\n", executable_name, full_path);
              found = true;
              break;
            }
          }
        }
      }
    }
    closedir(dir); // Close the directory when done with it
  }

  if (!found)
  {
    printf("%s: not found\n", executable_name);
  }

  // Free the memory we allocated with strdup()
  // This prevents memory leaks (memory that's allocated but never freed)
  free(paths_copy);
}

// Find an executable in PATH and return the full path (or NULL if not found)
// This is similar to find_path_of_type_cmd but returns a pointer instead of printing
// We use this for running external programs
char *find_executable_in_path(char *paths, char *executable_name)
{
  // Same pattern: copy the PATH string because we need to modify it
  char *paths_copy = strdup(paths);
  char *remaining = paths_copy;
  char *result = NULL;

  while (remaining != NULL)
  {
    char *delimiter = strchr(remaining, ':');

    // Extract current directory path
    char path[260];
    if (delimiter == NULL)
    {
      strcpy(path, remaining);
      remaining = NULL;
    }
    else
    {
      strncpy(path, remaining, delimiter - remaining);
      path[delimiter - remaining] = '\0';
      remaining = delimiter + 1;
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
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, de->d_name);

        // Check if file exists, is a regular file, and has execute permissions
        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR))
        {
          // strdup() allocates memory for the result on the heap
          // The caller is responsible for freeing this memory later!
          result = strdup(full_path);
          closedir(dir);
          free(paths_copy);
          return result; // Return early when found
        }
      }
    }
    closedir(dir);
  }

  // If we get here, executable was not found
  free(paths_copy);
  return NULL; // Return NULL to indicate not found
}

int main()
{
  // Flush after every print (forces output to appear immediately)
  setbuf(stdout, NULL);

  bool loop_continue = true;
  char user_input[1024];
  printf("$ ");
  while (loop_continue)
  {
    // fgets() reads a line from standard input (keyboard input)
    // It includes the newline character, which we remove next
    fgets(user_input, sizeof(user_input), stdin);

    // Remove the newline character that fgets() adds
    user_input[strlen(user_input) - 1] = '\0';

    // Check for exit command
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

    // Type command (shows if something is a builtin or where it's located)
    char typeinput[5];
    char executable_name[20];
    if (strcmp(memcpy(typeinput, user_input, 4), "type") == 0)
    {
      strcpy(executable_name, user_input + 5);

      // Check if it's a shell builtin
      if (strcmp(executable_name, "echo") == 0 || strcmp(executable_name, "exit") == 0 || strcmp(executable_name, "type") == 0)
      {
        printf("%s is a shell builtin\n", executable_name);
        printf("$ ");
        continue;
      }

      // Not a builtin, search for it in PATH
      char *paths = getenv("PATH"); // getenv() returns a pointer to the environment variable

      find_path_of_type_cmd(paths, executable_name);

      printf("$ ");
      continue;
    }

    // Handle external programs (anything that isn't "echo", "exit", or "type")
    char *paths = getenv("PATH");

    // Parse command name (first word) and arguments
    // strchr() finds the first space to split command from arguments
    char *space = strchr(user_input, ' ');
    char command_name[256];

#ifdef __unix__
    char *args[256];   // Array of pointers to store each argument
    int arg_count = 1; // Start at 1 because first arg is the program name itself

    if (space == NULL)
    {
      // No space, so the entire input is the command with no arguments
      strcpy(command_name, user_input);
      args[0] = command_name;
      args[1] = NULL; // NULL terminator tells execv() where args end
    }
    else
    {
      // Extract command name (part before the space)
      strncpy(command_name, user_input, space - user_input);
      command_name[space - user_input] = '\0';
      args[0] = command_name;

      // Parse additional arguments (separated by spaces)
      char args_copy[1024];
      strcpy(args_copy, space + 1);
      // strtok() splits a string by a delimiter (space in this case)
      char *current_arg = strtok(args_copy, " ");

      while (current_arg != NULL && arg_count < 255)
      {
        // strdup() allocates memory for each argument on the heap
        args[arg_count] = strdup(current_arg);
        arg_count++;
        current_arg = strtok(NULL, " "); // Get next token
      }
      args[arg_count] = NULL; // NULL terminator for execv()
    }
#else
    // On non-Unix systems, just extract the command name
    if (space == NULL)
    {
      strcpy(command_name, user_input);
    }
    else
    {
      strncpy(command_name, user_input, space - user_input);
      command_name[space - user_input] = '\0';
    }
#endif

    // Try to find and execute the program
    char *full_path = find_executable_in_path(paths, command_name);

    if (full_path != NULL)
    {
#ifdef __unix__
      // fork() creates a new process (copy of current process)
      // Returns 0 in the child process, child's PID in parent
      pid_t pid = fork();

      if (pid == 0)
      {
        // Child process: execute the program
        // execv() replaces this process with the new program
        // It takes the full path and the argument array
        execv(full_path, args);
        // If execv returns, there was an error (it normally doesn't return)
        perror("execv");
        exit(1);
      }
      else if (pid > 0)
      {
        // Parent process: wait for child to complete
        // waitpid() blocks until the child process finishes
        waitpid(pid, NULL, 0);
      }
      else
      {
        perror("fork");
      }
#else
      printf("External command execution not supported on this platform\n");
#endif

      free(full_path); // Free the memory allocated by find_executable_in_path()
    }
    else
    {
      printf("%s: command not found\n", command_name);
    }

    printf("$ ");
  }
  return 0;
}