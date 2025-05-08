#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
    int ping_pipe[2];
    int pong_pipe[2];
    pipe(ping_pipe);
    pipe(pong_pipe);

    if (fork() == 0)
    {
        close(ping_pipe[1]);
        close(pong_pipe[0]);

        read(ping_pipe[0], (int *)0, 1);
        fprintf(1, "%d: received ping\n", getpid());
        write(pong_pipe[1], "x", 1);
        exit(0);
    }
    else
    {
        close(ping_pipe[0]);
        close(pong_pipe[1]);

        write(ping_pipe[1], "x", 1);
        read(pong_pipe[0], (int *)0, 1);
        fprintf(1, "%d: received pong\n", getpid());
        exit(0);
    }
}