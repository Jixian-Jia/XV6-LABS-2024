#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BOUND 280

void sieve(int fd_read) __attribute__((noreturn));

void sieve(int fd_read)
{
    int prime, curr;
    int n;

    // Read the first number (prime)
    n = read(fd_read, &prime, sizeof(int));
    if (n != sizeof(int))
    {
        close(fd_read);
        exit(0);
    }

    fprintf(1, "prime %d\n", prime);

    int p[2];
    if (pipe(p) < 0)
    {
        fprintf(2, "pipe failed\n");
        close(fd_read);
        exit(1);
    }

    if (fork() == 0)
    {
        // Child sieve process
        close(p[1]);
        close(fd_read);
        sieve(p[0]); // recursive call
        // noreturn — no fallthrough
    }

    // Parent process
    close(p[0]);

    while ((n = read(fd_read, &curr, sizeof(int))) == sizeof(int))
    {
        if (curr % prime != 0)
            write(p[1], &curr, sizeof(int));
    }
    close(fd_read);
    close(p[1]);
    wait((void *)0);
    exit(0);
}

int main(void)
{
    int i;
    int p[2];
    if (pipe(p) < 0)
    {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    if (fork() == 0)
    {
        // Child sieve process
        close(p[1]);
        sieve(p[0]);
    }

    close(p[0]);
    for (i = 2; i < BOUND; i++)
    {
        write(p[1], &i, sizeof(int));
    }
    close(p[1]);
    wait((void *)0);
    exit(0);
}
