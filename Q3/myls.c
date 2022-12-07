#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <sys/sysmacros.h> //for major & minor number

#define DATE_MAXSIZE 64

// function prototype
void getFileInfo(char *filename);
char *getFileType(struct stat buf);
char *getUsername(struct stat buf);
char *getTime(time_t time);
char *getAccessPermission(struct stat buf);

int main(int argc, char *argv[])
{
    DIR *dp;
    struct dirent *direntp;
    char dirname[256];

    if (argc == 1)
    {
        // read current directory
        strcpy(dirname, ".");

        if ((dp = opendir(dirname)) == NULL)
        {
            printf("Fail to open directory %s\n", dirname);
            exit(1);
        }
        else
        {
            while ((direntp = readdir(dp)) != NULL)
            {
                printf("\nSource: %s\n", direntp->d_name);
                getFileInfo(direntp->d_name);
                printf("\n-------------------------------\n");
            }
        }
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            printf("\nSource: %s\n", argv[i]);
            getFileInfo(argv[i]);
            printf("\n-------------------------------\n");
        }
    }

    closedir(dp);
    exit(0);
}

// function implementation

// get file information
void getFileInfo(char *filename)
{
    int fd = 0;
    struct stat buf;
    struct group *grpptr;
    char *username;
    char *grpname;
    char *fileType;

    // open file for reading
    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        printf("Fail to open file %s\n", filename);
        exit(1);
    }

    // get file information
    if (lstat(filename, &buf) < 0) //use lstat so that can get symbolik link info
    {
        perror("Fail to get file informatino");
        exit(1);
    }

    // get username
    username = getUsername(buf);

    // get group name
    grpptr = getgrgid(buf.st_gid);
    grpname = grpptr->gr_name;

    // get file type
    fileType = getFileType(buf);

    // get access permission
    char *permission = getAccessPermission(buf);

    // get time
    char *accessTime = getTime(buf.st_atime);
    char *modificationTime = getTime(buf.st_mtime);
    char *changeTime = getTime(buf.st_ctime);

    // display output
    printf("User name: %s\n", username);
    printf("Group name: %s\n", grpname);
    printf("File type: %s\n", fileType);
    printf("Permission: %s\n", permission);
    printf("File size: %ld\n", buf.st_size);
    printf("I-node: %ld\n", buf.st_ino);
    printf("Device number: %d, %d\n", major(buf.st_dev), minor(buf.st_dev));
    printf("Number of links: %ld\n", buf.st_nlink);
    printf("Last access time: %s\n", accessTime);
    printf("Last modification: %s\n", modificationTime);
    printf("Last file status change: %s\n", changeTime);

    //another format of output
    /*
    printf("%s\t%s\t%s\t%s\t%ld bytes\t%ld\t%d, %d\t%ld\t%s\t%s\t%s\n", username, grpname, fileType, permission, buf.st_size, 
        buf.st_ino, major(buf.st_dev), minor(buf.st_dev), buf.st_nlink, accessTime, modificationTime, changeTime);
    */
    
}

// get username of owner
char *getUsername(struct stat buf)
{
    struct passwd *nameptr;
    char *username; // used to store username

    nameptr = getpwuid(buf.st_uid);
    username = nameptr->pw_name; // get username
    return username;
}

// get type of file
char *getFileType(struct stat buf)
{
    char *type;

    // get file type
    if (S_ISREG(buf.st_mode))
        type = "Regular file";
    else if (S_ISDIR(buf.st_mode))
        type = "Directory";
    else if (S_ISCHR(buf.st_mode))
        type = "Character special";
    else if (S_ISBLK(buf.st_mode))
        type = "Block special";
    else if (S_ISFIFO(buf.st_mode))
        type = "FIFO";
    else if (S_ISLNK(buf.st_mode))
        type = "Symbolic link";
    else if (S_ISSOCK(buf.st_mode))
        type = "Socket";
    else
        type = "Unknown file type";

    return type;
}

// get time information
char *getTime(time_t time)
{
    time_t t = time;
    struct tm *tmp;
    char buf[DATE_MAXSIZE];

    tmp = localtime(&t);
    strftime(buf, DATE_MAXSIZE, "%D, %r", tmp);

    char *ptr;
    ptr = malloc(sizeof(char) * DATE_MAXSIZE);
    strcpy(ptr, buf);

    return ptr;
}

// get full access permission
char *getAccessPermission(struct stat buf)
{
    char array[11];

    // check and get file type first
    if(S_ISREG(buf.st_mode))
        array[0] = '-';
    else if(S_ISDIR(buf.st_mode))
        array[0] = 'd';
    else if(S_ISCHR(buf.st_mode))
        array[0] = 'c';
    else if(S_ISBLK(buf.st_mode))
        array[0] = 'b';
    else if(S_ISFIFO(buf.st_mode))
        array[0] = 'p';
    else if(S_ISLNK(buf.st_mode))
        array[0] = 'l';
    else if(S_ISSOCK(buf.st_mode))
        array[0] = 's';

    // get access permission
    array[1] = (buf.st_mode & S_IRUSR) ? 'r' : '-'; // check if it is user read
    array[2] = (buf.st_mode & S_IWUSR) ? 'w' : '-'; // check if it is user write
    array[3] = (buf.st_mode & S_IXUSR) ? 'x' : '-'; // check if it is user execute
    array[4] = (buf.st_mode & S_IRGRP) ? 'r' : '-'; // check if it is group read
    array[5] = (buf.st_mode & S_IWGRP) ? 'w' : '-'; // check if it is group write
    array[6] = (buf.st_mode & S_IXGRP) ? 'x' : '-'; // check if it is group execute
    array[7] = (buf.st_mode & S_IROTH) ? 'r' : '-'; // check if it is other read
    array[8] = (buf.st_mode & S_IWOTH) ? 'w' : '-'; // check if it is other write
    array[9] = (buf.st_mode & S_IXOTH) ? 'x' : '-'; // check if it is other execute

    // return char pointer
    char *ptr;
    ptr = malloc(sizeof(char) * 11);
    strcpy(ptr, array);

    return ptr;
}