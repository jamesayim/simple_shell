#include "holberton.h"

/**
 * prompt - writes a prompt to stdout
 *
 * Return: 0 on success
 */
int prompt(void)
{
 char *prompt_str = "$ ";
 ssize_t writecount = 0;

 if (isatty(STDIN_FILENO) == 1)
 {
  writecount = write(STDOUT_FILENO, prompt_str, 2);
  if (writecount == -1)
   exit(EXIT_FAILURE);
 }
 return (0);
}

/**
 * _read - reads from stdin and stores the input in a buffer
 *
 * Return: a pointer to the buffer
 */
char *_read(void)
{
 ssize_t readcount = 0;
 size_t n = 0;
 char *buffer = NULL;

 readcount = getline(&buffer, &n, stdin);
 if (readcount == -1)
 {
  free(buffer);
  if (isatty(STDIN_FILENO) != 0)
   write(STDOUT_FILENO, "\n", 1);
  exit(EXIT_SUCCESS);
 }
 if (buffer[readcount - 1] == '\n' || buffer[readcount - 1] == '\t')
  buffer[readcount - 1] = '\0';
 for (int i = 0; buffer[i]; i++)
 {
  if (buffer[i] == '#' && buffer[i - 1] == ' ')
  {
   buffer[i] = '\0';
   break;
  }
 }
 return (buffer);
}

/**
 * _fullpathbuffer - finds the string to call execve on
 * @av: pointer to array of user strings
 * @PATH: pointer to PATH string
 * @copy: pointer to copy of PATH string
 *
 * Return: a pointer to string to call execve on
 */
char *_fullpathbuffer(char **av, char *PATH, char *copy)
{
 char *tok = NULL, *fullpathbuffer = NULL, *concatstr = NULL;
 static char tmp[256];
 int PATHcount = 0, fullpathflag = 0, z = 0, toklen = 0;
 struct stat h;

 copy = _strdup(PATH);
 PATHcount = _splitPATH(copy);
 tok = strtok(copy, ": =");
 while (tok != NULL)
 {
  concatstr = _concat(tmp, av, tok);
  if (stat(concatstr, &h) == 0)
  {
   fullpathbuffer = concatstr;
   fullpathflag = 1;
   break;
  }
  if (z < PATHcount - 2)
  {
   toklen = _strlen(tok);
   if (tok[toklen + 1] == ':')
   {
    if (stat(av[0], &h) == 0)
    {
     fullpathbuffer = av[0];
     fullpathflag = 1;
     break;
    }
   }
  }
  z++;
  tok = strtok(NULL, ":");
 }
 if (fullpathflag == 0)
  fullpathbuffer = av[0];
 free(copy);
 return (fullpathbuffer);
}

/**
 * check_builtins - Check if first user string is a built-in
 * @av: Pointer to array of user strings
 * @buffer: Pointer to user string
 * @exit_status: Exit status of execve
 *
 * Return: 1 if user string is equal to "env", or 0 otherwise.
 */
int check_builtins(char **av, char *buffer, int exit_status)
{
    int i;

    if (_strcmp(av[0], "env") == 0)
    {
        _env();
        for (i = 0; av[i]; i++)
            free(av[i]);
        free(av);
        free(buffer);
        return (1);
    }
    else if (_strcmp(av[0], "exit") == 0)
    {
        for (i = 0; av[i]; i++)
            free(av[i]);
        free(av);
        free(buffer);
        exit(exit_status);
    }
    else
        return (0);
}

/**
 * _fork_process - Create child process to execute based on user input
 * @av: Pointer to array of user strings
 * @buffer: Pointer to user string
 * @full_path_buffer: Pointer to user input
 *
 * Return: 0 on success
 */
int _fork_process(char **av, char *buffer, char *full_path_buffer)
{
    int i, status, result, exit_status = 0;
    pid_t pid;

    pid = fork();
    if (pid == -1)
    {
        perror("Error");
        exit(1);
    }
    if (pid == 0)
    {
        result =  execve(full_path_buffer, av, environ);
        if (result == -1)
        {
            perror(av[0]);
            for (i = 0; av[i]; i++)
                free(av[i]);
            free(av);
            free(buffer);
            exit(127);
        }
    }
    wait(&status);
    if (WIFEXITED(status))
    {
        exit_status = WEXITSTATUS(status);
    }
    for (i = 0; av[i]; i++)
        free(av[i]);
    free(av);
    free(buffer);
    return (exit_status);
}
