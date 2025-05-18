#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void run(char *command, int argc, char argv[MAXARG][32], char *new_line)
{
    int i = 0, j = 0;
    char buf[32];
    char *new_argv[MAXARG];
    int arg_idx = argc;

    // fill initial argv pointers
    for (int k = 0; k < argc; k++) {
        new_argv[k] = argv[k];
    }

    // parse new_line into arguments
    while (new_line[i] != '\0') {
        while (new_line[i] != ' ' && new_line[i] != '\0' && j < 31) {
            buf[j++] = new_line[i++];
        }
        buf[j] = '\0';
        if (j > 0) {
            strcpy(argv[arg_idx], buf);
            new_argv[arg_idx] = argv[arg_idx];
            arg_idx++;
            j = 0;
        }
        if (new_line[i] != '\0') i++;
    }

    new_argv[arg_idx] = 0;  // null-terminate for exec
    exec(command, new_argv);
    fprintf(2, "exec %s failed\n", command);
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(2, "xargs: wrong input\n");
        exit(1);
    }

    char buff[512];
    int i, j = 0;
    char new_command[32];
    char new_argv[MAXARG][32];
    // copy the command
    strcpy(new_command, argv[1]);

    // copy the arguments
    for (i = 2; i < argc; i++) {
        strcpy(new_argv[i - 1], argv[i]);
    }

    int new_argv_size = argc - 1;

    // read from stdin char by char
    while (read(0, &buff[j], 1) == 1) {
        if (buff[j] == '\n') {
            buff[j] = '\0';
            if (fork() == 0) {
                run(new_command, new_argv_size, new_argv, buff);
                exit(0);
            } else {
                wait((int *)0);
            }
            j = 0;
        } else {
            j++;
            if (j >= sizeof(buff) - 1) { 
                fprintf(2, "xargs: input line too long\n");
                exit(1);
            }
        }
    }

    exit(0);
}
