#include "myshell.h"

/* ------------------------------------------------------------------ */
/*  cd <directory>                                                      */
/*  Если аргумент не задан — выводим текущий каталог.                  */
/*  Меняем переменную среды PWD.                                       */
/* ------------------------------------------------------------------ */
void cmd_cd(char **args, int argc)
{
    if (argc < 2) {
        /* нет аргумента — вывести текущий каталог */
        char cwd[MAX_INPUT];
        if (getcwd(cwd, sizeof(cwd)))
            printf("%s\n", cwd);
        else
            perror("cd: getcwd");
        return;
    }

    if (chdir(args[1]) != 0) {
        fprintf(stderr, "cd: %s: %s\n", args[1], strerror(errno));
    } else {
        /* обновляем PWD */
        char cwd[MAX_INPUT];
        if (getcwd(cwd, sizeof(cwd)))
            setenv("PWD", cwd, 1);
    }
}

/* ------------------------------------------------------------------ */
/*  clr — очистка экрана                                               */
/* ------------------------------------------------------------------ */
void cmd_clr(void)
{
    printf("\033[H\033[J");
    fflush(stdout);
}

/* ------------------------------------------------------------------ */
/*  dir <directory> — вывод содержимого каталога                       */
/* ------------------------------------------------------------------ */
void cmd_dir(char **args, int argc)
{
    const char *path = (argc >= 2) ? args[1] : ".";
    DIR *dp = opendir(path);

    if (!dp) {
        fprintf(stderr, "dir: %s: %s\n", path, strerror(errno));
        return;
    }

    struct dirent *ep;
    while ((ep = readdir(dp)) != NULL)
        printf("%s\n", ep->d_name);

    closedir(dp);
}

/* ------------------------------------------------------------------ */
/*  environ — вывод всех переменных среды                              */
/* ------------------------------------------------------------------ */
void cmd_environ(void)
{
    for (char **env = environ; *env; env++)
        printf("%s\n", *env);
}

/* ------------------------------------------------------------------ */
/*  echo <comment> — вывод строки (множественные пробелы сжимаются)   */
/* ------------------------------------------------------------------ */
void cmd_echo(char **args, int argc)
{
    for (int i = 1; i < argc; i++) {
        if (i > 1) printf(" ");
        printf("%s", args[i]);
    }
    printf("\n");
}

/* ------------------------------------------------------------------ */
/*  help — вывод руководства пользователя из файла readme             */
/* ------------------------------------------------------------------ */
void cmd_help(void)
{
    char path[MAX_INPUT];
    char *shell_path = getenv("shell");

    if (shell_path) {
        strncpy(path, shell_path, sizeof(path) - 1);
        char *slash = strrchr(path, '/');
        if (slash) {
            *(slash + 1) = '\0';
            strncat(path, "readme", sizeof(path) - strlen(path) - 1);
        } else {
            strncpy(path, "readme", sizeof(path) - 1);
        }
    } else {
        strncpy(path, "readme", sizeof(path) - 1);
    }

    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "help: readme not found\n");
        return;
    }

    char line[MAX_INPUT];
    while (fgets(line, sizeof(line), f))
        printf("%s", line);

    fclose(f);
}

/* ------------------------------------------------------------------ */
/*  pause — ждём нажатия Enter                                         */
/* ------------------------------------------------------------------ */
void cmd_pause(void)
{
    printf("Press Enter to continue...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ------------------------------------------------------------------ */
/*  Запуск внешней программы через fork + exec                        */
/* ------------------------------------------------------------------ */
void run_external(char **args, char *shell_path)
{
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        setenv("parent", shell_path, 1);
        execvp(args[0], args);
        fprintf(stderr, "%s: command not found\n", args[0]);
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

/* ------------------------------------------------------------------ */
/*  Главный цикл оболочки                                              */
/* ------------------------------------------------------------------ */
void shell_loop(FILE *input, char *shell_path)
{
    char   line[MAX_INPUT];
    char  *args[MAX_ARGS];
    int    argc;
    int    is_tty = isatty(fileno(input));

    while (1) {
        if (is_tty) {
            char cwd[MAX_INPUT];
            if (getcwd(cwd, sizeof(cwd)))
                printf("%s $ ", cwd);
            else
                printf("myshell $ ");
            fflush(stdout);
        }

        if (!fgets(line, sizeof(line), input))
            break;

        line[strcspn(line, "\n")] = '\0';
        trim(line);

        if (strlen(line) == 0)
            continue;

        parse_args(line, args, &argc);

        if (argc == 0)
            continue;

        if (strcmp(args[0], "cd") == 0) {
            cmd_cd(args, argc);
        } else if (strcmp(args[0], "clr") == 0) {
            cmd_clr();
        } else if (strcmp(args[0], "dir") == 0) {
            cmd_dir(args, argc);
        } else if (strcmp(args[0], "environ") == 0) {
            cmd_environ();
        } else if (strcmp(args[0], "echo") == 0) {
            cmd_echo(args, argc);
        } else if (strcmp(args[0], "help") == 0) {
            cmd_help();
        } else if (strcmp(args[0], "pause") == 0) {
            cmd_pause();
        } else if (strcmp(args[0], "quit") == 0) {
            break;
        } else {
            run_external(args, shell_path);
        }
    }
}

/* ------------------------------------------------------------------ */
/*  main                                                               */
/* ------------------------------------------------------------------ */
int main(int argc, char *argv[])
{
    char shell_path[MAX_INPUT];
    if (realpath(argv[0], shell_path) == NULL)
        strncpy(shell_path, argv[0], sizeof(shell_path) - 1);

    setenv("shell", shell_path, 1);

    FILE *input = stdin;

    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "myshell: cannot open %s: %s\n",
                    argv[1], strerror(errno));
            return EXIT_FAILURE;
        }
    } else if (argc > 2) {
        fprintf(stderr, "Usage: %s [batchfile]\n", argv[0]);
        return EXIT_FAILURE;
    }

    shell_loop(input, shell_path);

    if (input != stdin)
        fclose(input);

    return EXIT_SUCCESS;
}
