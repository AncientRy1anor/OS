#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

#define MAX_INPUT  1024
#define MAX_ARGS   64
#define PROMPT_MAX 512

extern char **environ;

/* utility.c */
void  parse_args(char *line, char **args, int *argc);
void  trim(char *str);

/* внутренние команды */
void  cmd_cd(char **args, int argc);
void  cmd_clr(void);
void  cmd_dir(char **args, int argc);
void  cmd_environ(void);
void  cmd_echo(char **args, int argc);
void  cmd_help(void);
void  cmd_pause(void);

/* запуск внешней программы */
void  run_external(char **args, char *shell_path);

/* главный цикл */
void  shell_loop(FILE *input, char *shell_path);

#endif
