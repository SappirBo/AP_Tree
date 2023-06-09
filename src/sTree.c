#define _XOPEN_SOURCE 600 /* Get nftw() */
#include <ftw.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

#define DEBUG 0
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RESET   "\x1b[0m"


char *EMPTYSPACE = "    ";
char *DOWNLINE = "____";

static const char *parent_path = NULL;  // Stores the parent directory's path
static int dirCount = 0;  // Count of directories
static int fileCount = 0;  // Count of files


static void print_permissions(const struct stat *sbuf);
static void print_size(const struct stat *sbuf);
static int dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb);
static void print_owner_group(const struct stat *sbuf);


int main(int argc, char *argv[])
{
    /* Print New line for better view */
    printf("\n");

    int flags = FTW_PHYS;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s directory-path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (nftw(argv[1], dirTree, 10, flags) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    
    /* Print New line for better view */
    printf("\n");

    /* Print directory and file counts */
    printf("Total: %d directories and %d files\n", dirCount, fileCount);
    printf("\n");

    exit(EXIT_SUCCESS);
}


void print_permissions(const struct stat *sbuf)
{
    printf("[");
    printf((S_ISDIR(sbuf->st_mode)) ? "d" : "-");
    printf((sbuf->st_mode & S_IRUSR) ? "r" : "-");
    printf((sbuf->st_mode & S_IWUSR) ? "w" : "-");
    printf((sbuf->st_mode & S_IXUSR) ? "x" : "-");
    printf((sbuf->st_mode & S_IRGRP) ? "r" : "-");
    printf((sbuf->st_mode & S_IWGRP) ? "w" : "-");
    printf((sbuf->st_mode & S_IXGRP) ? "x" : "-");
    printf((sbuf->st_mode & S_IROTH) ? "r" : "-");
    printf((sbuf->st_mode & S_IWOTH) ? "w" : "-");
    printf((sbuf->st_mode & S_IXOTH) ? "x" : "-");
}

void print_owner_group(const struct stat *sbuf)
{
    struct passwd *pw = getpwuid(sbuf->st_uid);
    struct group *gr = getgrgid(sbuf->st_gid);
    if (pw)
        printf(" %s", pw->pw_name);
    else
        printf(" %d", sbuf->st_uid);
    if (gr)
        printf(" %s", gr->gr_name);
    else
        printf(" %d", sbuf->st_gid);
}

void print_size(const struct stat *sbuf)
{
    printf("%8ld", (long)sbuf->st_size);
    printf("]");
}

int dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
#if DEBUG
    printf("Level: %d, ", ftwb->level);
    printf("Base: %d ,", ftwb->base);
#endif

    char fileMod = '-';
    if (type == FTW_NS) {
        printf("?");
    } else {
        switch (sbuf->st_mode & S_IFMT) {
        case S_IFREG: fileMod = '-'; break;
        case S_IFDIR: fileMod = 'd'; break;
        case S_IFCHR: fileMod = 'c'; break;
        case S_IFBLK: fileMod = 'b'; break;
        case S_IFLNK: fileMod = 'l'; break;
        case S_IFIFO: fileMod = 'p'; break;
        case S_IFSOCK: fileMod = 's'; break;
        default: fileMod = '?'; break;
        }
    }

#if DEBUG
    printf("STAT: %c", fileMod);
    printf(" | ");
#endif

    // Check if the current directory's path matches the parent directory's path
    if (parent_path && strncmp(parent_path, pathname, ftwb->base) == 0) {
        for (int i = 0; i < (ftwb->level) - 1; i++) {
            printf("    ");
        }
        printf("|____");
    } else {
        parent_path = pathname;
    }

    print_permissions(sbuf);
    print_owner_group(sbuf);
    print_size(sbuf);
    if (fileMod == 'd') {
        printf(COLOR_BLUE" %s\n", &pathname[ftwb->base]);
        printf(COLOR_RESET "");
        dirCount++;
    } else {
        printf(" %s\n", &pathname[ftwb->base]);
        fileCount++;
    }

    return 0;
}
