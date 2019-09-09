#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "vector.h"
#include <time.h>
#include <grp.h>
#include <pwd.h>


// struct containing all file's info
typedef struct _node {
 struct dirent * file;
 struct stat info;
 char namebuff[512+1]; // stores the name
 //for some reason -l would not print proper names from file->d_name
} NODE;

NODE * makeNode(struct dirent * d, struct stat s)
{ NODE * ret = malloc(sizeof(NODE));
  ret->file = d;
  ret->info = s;
  strcpy(ret->namebuff, d->d_name);
  return ret; }

// creates a vector containing all the files within the given parthname
VECTOR * makeList(char *pathname, const int flags)
{ DIR *dirp = opendir(pathname);
  if(dirp == NULL) return NULL;
  VECTOR * ret = makeVec();
  struct dirent * dirBuf;
  struct stat statBuf;
  char charBuf[512+1]; // longest name has to be under 512 chars
  while((dirBuf = readdir(dirp)) != NULL)
  { sprintf(charBuf, "%s/%s", pathname, dirBuf->d_name);
    lstat(charBuf, &statBuf);
    if(flags%10 == 0 && dirBuf->d_name[0] == '.') continue;
    NODE * temp = makeNode(dirBuf, statBuf);
    addAtBackVec(ret, (void*)temp); } //adds node to list
  closedir(dirp);
//  freeEmptySpaceVec(ret);
  return ret; }

int compareNode(void *a, void *b)
{ if(a == NULL && b == NULL) return 0;
  else if(a == NULL) return 1;
  else if(b == NULL) return -1;
  else if(((NODE*)a)->info.st_size > ((NODE*)b)->info.st_size) return 1;
  else if(((NODE*)a)->info.st_size < ((NODE*)b)->info.st_size) return -1;
  else return 0; }

// print func for -Ua
void printNode(void * d)
{ if(d == NULL) printf("empty\n");
  else
  { NODE * n = (NODE*)d;
    printf("%s\n", n->file->d_name);
    fflush(stdout); } }

// print func for -sUa
void printNodeS(void * d)
{ if(d == NULL) printf("empty\n");
  else
  { NODE * n = (NODE*)d;
    printf("%8lu ", n->info.st_size);
    printf(" %s\n", n->file->d_name);
    fflush(stdout); } }

// helper funcs for printNodeL(...)
void printPermissions(const mode_t st_mode)
{ //Is there a better way of doing this?
  char str[] = "----------";
  if(st_mode & S_IFDIR) str[0] = 'd';
  if(st_mode & S_IRUSR) str[1] = 'r';
  if(st_mode & S_IWUSR) str[2] = 'w';
  if(st_mode & S_IXUSR) str[3] = 'x';
  if(st_mode & S_IRGRP) str[4] = 'r';
  if(st_mode & S_IWGRP) str[5] = 'w';
  if(st_mode & S_IXGRP) str[6] = 'x';
  if(st_mode & S_IROTH) str[7] = 'r';
  if(st_mode & S_IWOTH) str[8] = 'w';
  if(st_mode & S_IXOTH) str[9] = 'x';
  printf("%s", str);
  fflush(stdout); }

void printUid(const uid_t st_uid)
{ struct passwd * pwd = getpwuid(st_uid);
  printf(" %8s", pwd->pw_name);
  fflush(stdout); }

void printGid(const gid_t st_gid)
{ struct group * grp = getgrgid(st_gid);
  printf(" %8s", grp->gr_name);
  fflush(stdout); }

void printClockTime(const time_t t)
{ struct tm * lt = localtime(&t);
  printf(" %2d:%.2d",lt->tm_hour,lt->tm_min);
  fflush(stdout); }

void printCalendarDate(const time_t t)
{ const char * months[] =
  { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL };
  struct tm * lt = localtime(&t);
  printf(" %s %2d %d",months[lt->tm_mon],lt->tm_mday, lt->tm_year+1900);
  fflush(stdout); }

// print func for -lsUa
void printNodeL(void * d)
{ if(d == NULL) printf("empty\n");
  else
  { NODE * n = (NODE*)d;
    printPermissions(n->info.st_mode);
    printf("%4lu ", n->info.st_nlink);
    printUid(n->info.st_uid);
    printGid(n->info.st_gid);
    printf(" %6lu ", n->info.st_size);
    printCalendarDate(n->info.st_mtime);
    printClockTime(n->info.st_mtime);
    printf(" %s\n", n->namebuff);
    fflush(stdout); } }

// prints command usage and returns error code
int usage(char* argv[])
{ fprintf(stderr, "Usage: %s [-lsUa] <pathname(s)>\n", argv[0]);
  return -1; }


int main(int argc, char* argv[])
{ int i, opt, flags = 0;

  /* flags order = lsUa
     if flags = 1111 all flags are set */

  // sets all 4 command options if applicable

  while ((opt = getopt(argc, argv, "Usla"))!= -1)
  { switch (opt)
    { case 'l':
        flags += 1000;
        break;
      case 's':
        flags += 100;
        break;
      case 'U':
        flags += 10;
        break;
      case 'a':
        flags += 1;
        break;
      default:
        break; } }

  VECTOR * list;
  if(argc == 3) list = makeList(argv[2], flags);
  else if(flags == 0 && argc == 2) list = makeList(argv[1], flags);
  else list = makeList(".", flags);
  if(list == NULL) return usage(argv);

  /* sorts if -s flag is present */
  if(flags/100%10) quickSortVec(list, compareNode);

  /* main print functions */
  if(flags <= 11)
  { printVec(list, printNode); }
  else if(flags <= 111)
  { printVec(list, printNodeS); }
  else
  { printVec(list, printNodeL); }
}

