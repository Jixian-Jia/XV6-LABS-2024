#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char *get_name(char *path)
{

    char *last_slash = (char *)0;
    for (char *p = path; *p != '\0'; ++p)
    {
        if (*p == '/')
        {
            last_slash = p;
        }
    }

    // If we found a '/', return the char after it, else the whole path
    return (last_slash) ? (last_slash + 1) : path;
}
void find(char *path, char *name)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_FILE:
    case T_DEVICE:
        if (strcmp(get_name(path), name) == 0)
        {
            fprintf(1, "%s\n", path);
        }
        break;

    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("find: path too long\n");
            break;
        }

        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;

            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = '\0';

            if (stat(buf, &st) < 0)
            {
                printf("find: cannot stat %s\n", buf);
                continue;
            }

            find(buf, name);
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "find: wrong input\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}