#define _XOPEN_SOURCE 600 /* Get nftw() */
#include <ftw.h>
#include <sys/types.h>    /* Type definitions used by many programs */
#include <stdio.h>        /* Standard I/O functions */
#include <stdlib.h>       /* Prototypes of commonly used library functions, plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>       /* Prototypes for many system calls */
#include <errno.h>        /* Declares errno and defines error constants */
#include <string.h>       /* Commonly used string-handling functions */

#define DEBUG 1

char * EMPTYSPACE = "    ";
char * DOWNLINE   = "____";
int last_level = 0;

static int dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{

#if DEBUG
    // printf("PATH: %s, ", pathname);
    printf("Level: %d, ", ftwb->level);
    // printf("Base: %d ,",  ftwb->base);
    // printf("Path name: %s \n",&pathname[ftwb->base]);
#endif

    char fileMod = '-';
    if (type == FTW_NS) {                  /* Could not stat() file */
        printf("?");
    } else {
        switch (sbuf->st_mode & S_IFMT) {  /* Print file type */
        case S_IFREG:  fileMod = '-'; break;
        case S_IFDIR:  fileMod = 'd'; break;
        case S_IFCHR:  fileMod = 'c'; break;
        case S_IFBLK:  fileMod = 'b'; break;
        case S_IFLNK:  fileMod = 'l'; break;
        case S_IFIFO:  fileMod = 'p'; break;
        case S_IFSOCK: fileMod = 's'; break;
        default:       fileMod = '?'; break; /* Should never happen (on Linux) */
        }
    }

    printf("STAT: %c",fileMod);
    printf(" | ");
    

	
    for(int i=0; i < (ftwb->level); i++)
    {
        if(i<ftwb->level-1)
        {
            printf("%s",EMPTYSPACE);
        }
        else
        {
            printf("|");
            printf("%s",DOWNLINE);
        }
    }

    /* Print basename */
    printf("[]%s\n",  &pathname[ftwb->base]);   
    
    /* Tell nftw() to continue */
    return 0;
}

int main(int argc, char *argv[])
{
    int flags = 0;
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s directory-path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (nftw(argv[1], dirTree, 10, flags) == -1) 
    {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}