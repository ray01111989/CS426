#include<stdio.h>
#include<stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/uio.h>
//#include <sys/sendfile.h>


static uid_t euid, ruid;


/* Restore the effective UID to its original value. */

void
do_setuid (void)
{
  int status;

#ifdef _POSIX_SAVED_IDS
  status = seteuid (euid);
#else
    status = setreuid (ruid, euid);
#endif
  if (status < 0) {
    fprintf (stderr, "Couldn't set uid.\n");
    exit (status);
    }
}


/* Set the effective UID to the real UID. */

void
undo_setuid (void)
{
  int status;

#ifdef _POSIX_SAVED_IDS
  status = seteuid (ruid);
#else
  status = setreuid (euid, ruid);
#endif
  if (status < 0) {
    fprintf (stderr, "Couldn't set uid.\n");
    exit (status);
    }
}

int readline(FILE *f, char *buffer, size_t len)
{
   char c; 
   int i;

   memset(buffer, 0, len);

   for (i = 0; i < len; i++)
   {   
      int c = fgetc(f); 

      if (!feof(f)) 
      {   
         if (c == '\r')
            buffer[i] = 0;
         else if (c == '\n')
         {   
            buffer[i] = 0;

            return i+1;
         }   
         else
            buffer[i] = c; 
      }   
      else
      {   
         //fprintf(stderr, "read_line(): recv returned %d\n", c);
         return -1; 
      }   
   }   

   return -1; 
}
bool symlink_exists(const char* path)
{
    struct stat buf;
    int result;

    result = lstat("test.c", &buf);

    return (buf.st_nlink > 1);
}

void testX(const char* path)
{
    bool exists = symlink_exists(path);

    printf("%s The symbolic link does%s exist.\n", path, exists ? "" : " not");
}
 char* concat(const char *s1, const char *s2){
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
    }
int main(int argc, char *argv[]){
   if(argc < 3){
       printf("This file do not exist! \n");
       printf("Please enter distanction name file");
   }
   testX("test.c");

    /* Remember the real and effective user IDs.  */
  ruid = getuid ();
  euid = geteuid ();
  undo_setuid ();

    char* s = concat("test.c", ".acl");

    if( access( s, F_OK ) != -1 ) {
        // file exists
        printf("This file Exists \n");
    } else {
        // file doesn't exist
        printf("This file does not Exist \n");
    }
    struct stat buf;
    int x;
    char user[100], permission;
    char filename[] = "test.c.acl";
    char* buff = malloc(sizeof(1024));
    FILE *file = fopen ( filename, "r" );
    readline(file, buff,1024);
    
    if(sscanf(buff, "%s %c",user,&permission) == 2){
    printf("User: %s\n", user);
    printf("Permission: %c\n", permission);
    } else
    {
        printf("The file has wrong format \n");
    }

  printf("The REAL UID =: %d\n", ruid);
  printf("The EFFECTIVE UID =: %d\n", euid);
    
   
   
 int read_fd;
 int write_fd;
 struct stat stat_buf;
 off_t offset = 0;

 /* Open the input file. */
 read_fd = open (argv[1], O_RDONLY);
 /* Stat the input file to obtain its size. */
 fstat (read_fd, &stat_buf);
 /* Open the output file for writing, with the same permissions as the
   source file. */
 write_fd = open (argv[2], O_WRONLY | O_CREAT, stat_buf.st_mode);
 /* Blast the bytes from one file to the other. */
 //sendfile (write_fd, read_fd, &offset, stat_buf.st_size);
 int sendfile(int fd, int s, off_t offset, off_t *len, struct sf_hdtr *hdtr, int flags);
 /* Close up. */
 close (read_fd);
 close (write_fd);

    return 0;

     
}
