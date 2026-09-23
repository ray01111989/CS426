#ifndef SCOPY_H
#define SCOPY_H

// Helper functions for scopy.c. They live in a header because the whole program is a single .c file.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/sendfile.h> // Linux only

char **global_argv;          // the program's arguments, saved so helper functions can read them
static uid_t euid, ruid;     // effective user id (the owner of the setuid program) and real user id (whoever ran it)

/* Restore the effective UID to its original value. (Currently not called anywhere; see the README notes.) */
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
/* Set the effective UID to the real UID, so the program runs with the privileges of the user who started it. */
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
/* Check that the user gave both file names: the file to copy and where to copy it to */
bool correctArguments(int arguments)
{
        if (arguments < 3)
        {
        fprintf(stderr, "* Please enter destination name file *\n");
        return false;
        }
        return true;
}
/* Checks that filename is a plain file with a single hard link (so it cannot be a symlink to somewhere else).
   Exits if the file cannot be examined or is not a regular file; returns true if it has extra links. */
bool symlink_exists(const char* filename)
{
        struct stat buf;
        // lstat looks at the link itself instead of following it. (It used to check global_argv[1] no matter
        // which file name was passed in, so the .acl file was never actually examined.)
        if (lstat(filename, &buf) < 0)
        {
        perror("stat");
        exit(EXIT_FAILURE);
        }

        // Only regular files may be used; a symlink shows up here as a different file type
        if(!((buf.st_mode & S_IFMT) == S_IFREG)){
                printf("Only a regular file can be copy!!!\n");
		exit(1);
        }
        // More than one hard link means the same data is reachable under another name
	if (buf.st_nlink > 1){
        fprintf(stderr, "%s has more than one hard link!\n", filename);
        return true;
	}

        return false;
}

/* Checks that the .acl file exists */
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
/* Checks who owns the files: the data file must belong to the effective user (the person who installed scopy),
   and the .acl file must belong to the same person as the scopy program itself.
   (The parameters used to be named the other way round from how main() passes them; they are named for what they hold now.) */
bool isCorrectOwnership(char* aclPath, char* dataPath, char* programPath){
    struct stat aclInfo, dataInfo, scopyInfo;

    // If any of the three files cannot be examined, the ownership cannot be confirmed, so refuse
    if (stat(dataPath, &dataInfo) < 0 || stat(aclPath, &aclInfo) < 0 || stat(programPath, &scopyInfo) < 0){
        perror("stat");
        return false;
    }

    if(!((euid == dataInfo.st_uid) && (aclInfo.st_uid == scopyInfo.st_uid))){
        fprintf(stderr, "Files don't share the same owner!\n");
        return false;
    }
    return true;
}


#endif
