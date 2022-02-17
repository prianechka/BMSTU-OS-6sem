#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "color.h"

#define NOT_A_DIRECTORY 1
#define IS_DIRECTORY 2
#define DENIED 3

typedef int __handler_fn_t(const char *filename,  const struct stat *st, int depth, int type);

static int dopath(const char *filename, int depth, __handler_fn_t *my_handler) 
{
    struct stat statbuf;
    DIR *directory;
    int status;
    struct dirent *cur_dir;

    if (lstat(filename, &statbuf) == -1) 
    {
        return EXIT_FAILURE;
    }

    if (S_ISDIR(statbuf.st_mode) == 0) 
    {
        return my_handler(filename, &statbuf, depth, NOT_A_DIRECTORY);
    }

    if ((status = my_handler(filename, &statbuf, depth, IS_DIRECTORY)) != EXIT_SUCCESS) 
    {
        return status;
    }

    if ((directory = opendir(filename)) == NULL) 
    {
        return my_handler(filename, &statbuf, depth, DENIED);
    }

    chdir(filename);

    while ((cur_dir = readdir(directory)) != NULL) 
    {
        if ((strcmp(cur_dir->d_name, ".") == 0) | (strcmp(cur_dir->d_name, "..") == 0))
            continue;
        if (status = dopath(cur_dir->d_name, depth + 1, my_handler) != EXIT_SUCCESS)
        {
            return status;
        }
    }
    chdir("..");

    if (closedir(directory) == -1)
    {
        fprintf(stderr, "closedir(%s): %s", filename, strerror(errno));
    }

    return status;
}

static int work_handler(const char *filename, const struct stat *st, 
                        int depth, int type)
{
    for (int i = 0; i + 1 < depth; i++) 
    {
        printf("|   ");
    }

    const char *s = "";

    switch (type) 
    {
        case NOT_A_DIRECTORY:
            switch (st->st_mode & S_IFMT) 
            {
                case S_IFREG:
                    s = st->st_mode & S_IXUSR ? COLOR_EXE : COLOR_REG;
                    break;
                case S_IFBLK:
                    s = COLOR_BLK;
                    break;
                case S_IFCHR:
                    s = COLOR_CHR;
                    break;
                case S_IFIFO:
                    s = COLOR_FIFO;
                    break;
                case S_IFLNK:
                    s = COLOR_LINK;
                    break;
                case S_IFSOCK:
                    s = COLOR_SOCK;
                    break;
                case S_IFDIR:
                    fprintf(stderr, "Dir file type is not expected\n");
                    return EXIT_FAILURE;
                default:
                    fprintf(stderr, "Unknown file type '%s': %d\n", filename, st->st_mode & S_IFMT);
                    return EXIT_FAILURE;
            }
            printf("|--- %s%s%s %d\n", s, filename, COLOR_RESET, st->st_ino);
            break;
        case IS_DIRECTORY:
            printf("%s%s%s%s\n", depth ? "|--- " : "", COLOR_DIR, filename, COLOR_RESET);
            break;
        case DENIED:
            fprintf(stderr, "Can't read directory %s: %s\n", filename, strerror(errno));
            return EXIT_FAILURE;
        default:
            fprintf(stderr, "Unknown file type: %d\n", type);
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(const int argc, const char **argv) 
{
    srand(time(NULL));
    
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <dir>\n", argv[0]);
        return EXIT_FAILURE;
    }

    return dopath(argv[1], 0, work_handler);
}
