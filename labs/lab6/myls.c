// myls.c ... my very own "ls" implementation

#include <stdlib.h>
#include <stdio.h>
#include <bsd/string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAXDIRNAME 100
#define MAXFNAME   200
#define MAXNAME    20

char *rwxmode(mode_t, char *);
char *username(uid_t, char *);
char *groupname(gid_t, char *);

int main(int argc, char *argv[])
{
   // string buffers for various names
   char dirname[MAXDIRNAME];
   char name[MAXDIRNAME+MAXNAME+1];
   char uname[MAXNAME+1]; // UNCOMMENT this line
   char gname[MAXNAME+1]; // UNCOMMENT this line
   char mode[MAXNAME+1]; // UNCOMMENT this line

   // collect the directory name, with "." as default
   if (argc < 2)
      strlcpy(dirname, ".", MAXDIRNAME);
   else
      strlcpy(dirname, argv[1], MAXDIRNAME);

   // check that the name really is a directory
   struct stat info;
   if (stat(dirname, &info) < 0)
      { perror(argv[0]); exit(EXIT_FAILURE); }
   if ((info.st_mode & S_IFMT) != S_IFDIR)
      { fprintf(stderr, "%s: Not a directory\n",argv[0]); exit(EXIT_FAILURE); }

   // open the directory to start reading
   DIR *df; // UNCOMMENT this line
   // ... TODO ...
   df = opendir(dirname);
  
   // read directory entries
   struct dirent *entry; // UNCOMMENT this line
   // ... TODO ...
   entry = readdir(df);
   while(entry != NULL)
   {
       if(entry->d_name[0] == '.') 
       {
           entry = readdir(df);
           continue;
       }
       strcpy(name, dirname);
       strcat(name, "/");
       strcat(name, entry->d_name);
       //printf("%s\n", name);
       if(lstat(name, &info) < 0)
          { perror(argv[0]); exit(EXIT_FAILURE); }
       
       printf("%s  %-8.8s %-8.8s %8lld  %s\n",
       rwxmode(info.st_mode, mode),
       username(info.st_uid, uname),
       groupname(info.st_gid, gname),
       (long long)info.st_size,
       entry->d_name);
        
       entry = readdir(df);
   }

   // finish up
   closedir(df); // UNCOMMENT this line
   return EXIT_SUCCESS;
}

// convert octal mode to -rwxrwxrwx string
char *rwxmode(mode_t mode, char *str)
{
    // ... TODO ...
    //printf("%x\n", mode);
    //printf("%d\n", S_ISLNK(S_IFLNK));
    if (S_ISDIR(mode)) str[0] = 'd';
    else if (S_ISLNK(mode)) str[0] = 'l';
    else if (S_ISREG(mode)) str[0] = '-';
    else str[0] = '?';
    //str[0] = ((S_ISDIR(mode)) ? 'd' : '-');
    str[1] = ((mode & S_IRUSR) ? 'r' : '-');
    str[2] = ((mode & S_IWUSR) ? 'w' : '-');
    str[3] = ((mode & S_IXUSR) ? 'x' : '-');
    str[4] = ((mode & S_IRGRP) ? 'r' : '-');
    str[5] = ((mode & S_IWGRP) ? 'w' : '-');
    str[6] = ((mode & S_IXGRP) ? 'x' : '-');
    str[7] = ((mode & S_IROTH) ? 'r' : '-');
    str[8] = ((mode & S_IWOTH) ? 'w' : '-');
    str[9] = ((mode & S_IXOTH) ? 'x' : '-');
    str[10] = '\0';
    return str;
}

// convert user id to user name
char *username(uid_t uid, char *name)
{
   struct passwd *uinfo = getpwuid(uid);
   if (uinfo == NULL)
      snprintf(name, MAXNAME, "%d?", (int)uid);
   else
      snprintf(name, MAXNAME, "%s", uinfo->pw_name);
   return name;
}

// convert group id to group name
char *groupname(gid_t gid, char *name)
{
   struct group *ginfo = getgrgid(gid);
   if (ginfo == NULL)
      snprintf(name, MAXNAME, "%d?", (int)gid);
   else
      snprintf(name, MAXNAME, "%s", ginfo->gr_name);
   return name;
}
