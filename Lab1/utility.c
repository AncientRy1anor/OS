#include "myshell.h"

void trim(char *str)
{
    if (!str) return;

    /* убираем пробелы в конце */
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t'))
        str[--len] = '\0';

    /* убираем пробелы в начале, сдвигая строку */
    char *start = str;
    while (*start == ' ' || *start == '\t')
        start++;

    if (start != str)
        memmove(str, start, strlen(start) + 1);
}

void parse_args(char *line, char **args, int *argc)
{
    *argc = 0;
    char *token = strtok(line, " \t");

    while (token != NULL && *argc < MAX_ARGS - 1) {
        args[(*argc)++] = token;
        token = strtok(NULL, " \t");
    }

    args[*argc] = NULL;   /* завершающий NULL для execvp */
}
