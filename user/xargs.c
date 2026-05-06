#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "usage: xargs cmd\n");
        exit(1);
    }

    char buf[512];
    int i = 0;

    // read one line at a time
    while (read(0, &buf[i], 1) == 1)
    {
        if (buf[i] == '\n')
        {
            buf[i] = 0; 

            if (fork() == 0)
            {
                char *new_argv[MAXARG];

                // copy original args
                int j;
                for (j = 1; j < argc; j++)
                {
                    new_argv[j - 1] = argv[j];
                }

                // append input line as last argument
                new_argv[j - 1] = buf;
                new_argv[j] = 0;

                exec(argv[1], new_argv);

                fprintf(2, "exec failed\n");
                exit(1);
            }

            wait(0);
            i = 0;
        }
        else
        {
            i++;
        }
    }

    exit(0);
}