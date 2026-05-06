#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *path, char *target)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0)
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

    // extract basename of path
    char *name = path + strlen(path);
    while (name >= path && *name != '/')
        name--;
    name++;

    if (st.type == T_FILE || st.type == T_DEVICE)
    {
        if (strcmp(name, target) == 0)
            printf("%s\n", path);
        close(fd);
        return;
    }

    if (st.type == T_DIR)
    {
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("find: path too long\n");
            close(fd);
            return;
        }

        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
                
            char dname[DIRSIZ + 1];
            memmove(dname, de.name, DIRSIZ);
            dname[DIRSIZ] = 0;

            // skip "." and ".."
            if (strcmp(dname, ".") == 0 || strcmp(dname, "..") == 0)
                continue;

            // build full path
            strcpy(p, dname);

            // recurse
            find(buf, target);
        }
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "usage: find path filename\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}