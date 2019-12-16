#ifndef SCOPY_H
#define SCOPY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>

char **global_argv;
static uid_t euid, ruid;
/* Restore the effective UID to its original value. */
void do_setuid (void)
{
        int status;
        #ifdef _POSIX_SAVED_IDS
        status = seteuid (euid);
        #else
        status = setreuid (ruid, euid);
        #endif
        if (status < 0)
        {
        fprintf (stderr, "Couldn't set uid.\n");
        exit (status);
        }
}
/* Set the effective UID to the real UID. */
void undo_setuid (void)
{
        int status;
        #ifdef _POSIX_SAVED_IDS
        status = seteuid (ruid);
        #else
        status = setreuid (euid, ruid);
        #endif
        if (status < 0)
        {
        fprintf (stderr, "Couldn't set uid.\n");
        exit (status);
        }
}

#include <errno.h>
/* Check if the arguments are given the full bath correct */
bool correctArguments(int arguments)
{
        if (arguments < 3)
        {
        fprintf(stderr, "* Please enter destination name file *\n");
        return false;
        }
        return true;
}
/* This function will check if the files are symlinked */
bool symlink_exists(const char* filename)
{
        struct stat buf;
        if (lstat(global_argv[1], &buf) < 0)
        {
        perror("stat");
        exit(EXIT_FAILURE);
        }
        
        if(!((buf.st_mode & S_IFMT) == S_IFREG)){
                printf("Only a regular file can be copy!!!\n");
		exit(1);
        }
	if (buf.st_nlink > 1){
        fprintf(stderr, "%s is a symlink file!\n", filename);
        return true;
	}

        return false;
}

/* This function to check if the .acl file exists */
bool ACLExists(char* s)
{
	struct stat   buffer;
  	if(stat (s, &buffer) == 0){
		return true;
	}else{
		printf("** The .acl file does not Exists **\n");
		return false;
	}
      
}
bool isCorrectOwnership(char* targetFile, char* aclFile, char* programName){
    struct stat aclFileInfo, fileInfo, scopyInfo;

    stat(aclFile, &aclFileInfo);
    stat(targetFile, &fileInfo);
    stat(programName, &scopyInfo);

    if(!((euid == aclFileInfo.st_uid) && (fileInfo.st_uid == scopyInfo.st_uid))){
        fprintf(stderr, "Files don't share the same owner!\n");
        return false;
    }
    return true;
}


#endif

