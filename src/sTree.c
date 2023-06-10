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
#define MAX_NUM 512 // Means it goes deep as 512 files 


char *EMPTYSPACE = "    ";
char *DOWNLINE = "____";

// static const char *parent_path = NULL;  // Stores the parent directory's path
static int dirCount = 0;  // Count of directories
static int fileCount = 0;  // Count of files
// Keep track of the deepest level visited in the last iteration.
static int max_level = -1;
// Keep track of the deepest level where a child was encountered for each level.
static int last_child_level[1024] = {-1};
// Keep track of the previously visited level.
static int prev_level = -1;

static int number_set[MAX_NUM] = {0};  // All elements initialized to false


static void print_permissions(const struct stat *sbuf);
static void print_size(const struct stat *sbuf);
static int dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb);
static void print_owner_group(const struct stat *sbuf);
static int countSet(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb);
static void printSet();


int main(int argc, char *argv[])
{
    /* Print New line for better view */
    printf("\n");

    int flags = FTW_PHYS;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s directory-path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (nftw(argv[1], countSet, 10, flags) == -1) {
        perror("nftw");
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

int countSet(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb){
    // printf("------------ Level = %d\n", ftwb->level);
    number_set[ftwb->level]++;
    return 0;
}

void printSet(){
    for(int i=0; i<MAX_NUM; i++)
    {
        if(number_set[i] > 0){
            printf("number_set[%d] = %d\n",i,number_set[i]);
        }
    }
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


    // Update the deepest level.
    max_level = ftwb->level;

    if(prev_level == ftwb->level && ftwb->level != 0)
    {
        prev_level -=1;
    }
    else if(prev_level > ftwb->level)
    {
        if(prev_level - 2 < 0){prev_level==0;}
        else{prev_level -= 2;}
    }



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
    printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    printf("[DEBUG] ");
    printf("Level: %d, ", ftwb->level);
    printf("Base: %d ,", ftwb->base);
    printf("STAT: %c", fileMod);
    printf("path: %s", pathname);
    printf(" | \n");
    printf("[DEBUG] ");
    printf("max_level: %d, ", max_level);
    printf("prev_level: %d, ", prev_level);
    printf(" | \n");
    printf("[DEBUG] ");
    printf("number_set[%d]: %d, ",ftwb->level, number_set[ftwb->level]);
    printf(" | \n");
    
#endif

    // Only print `|` if the current level is less than the previously visited level.
    for (int i = 0; i < ftwb->level; i++) {
        if (i < prev_level +1) {
            if(i == prev_level){
                printf("|___");
            }else{
                if(number_set[prev_level] > 1){
                    printf("|   ");
                }else{
                    printf("    ");   
                }    
            }
        } else {
            printf("    ");
        }
    }

    // Update the last child level of the current level.
    last_child_level[ftwb->level - 1] = max_level;
    // Update the previously visited level.
    if(fileMod == 'd'){
        prev_level = ftwb->level;
    }

    number_set[ftwb->level]--;
    

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
