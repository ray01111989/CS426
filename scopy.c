#include "functions.h"

// scopy: copy a file for another user if the file's .acl list allows it.
// Usage: scopy <source file> <destination file>
// The source file "data.txt" must have an access list "data.txt.acl" next to it: one line per allowed user,
// in the form "username r".

#define MAX_ACL_ENTRIES 200   // most lines read from the .acl file
#define NAME_LEN 200          // room for one user name, including the ending '\0'

/* This is the where main function start */
int main(int argc, char *argv[])
	{
	global_argv = argv;
	/*check all arguments*/
	if(!correctArguments(argc))
        return -1;
  	/*Remember the real and effective user IDs.  */
  	ruid = getuid ();
	euid = geteuid ();
	undo_setuid ();
	char* s = argv[1];
	/* The allowed user names read from the .acl file, and the permission letter that follows each one */
	char user[MAX_ACL_ENTRIES][NAME_LEN], permission[MAX_ACL_ENTRIES];
	// Build the .acl file name: "<source file>.acl"
	char* filename = (char*)malloc(strlen(argv[1]) * sizeof(char) + strlen(".acl") + 1);
	if (filename == NULL)
	{
		fprintf(stderr, "Out of memory\n");
		return 1;
	}
	strcpy(filename, argv[1]);
	strcat(filename, ".acl");
	// check if symlink.
	if (symlink_exists(s) || symlink_exists(filename))
	return -1;
	// check if the file acl exist....
	if (!ACLExists(filename))
	return -1;
	if(!isCorrectOwnership(filename, s, argv[0]))
        return -1;
	/* A buffer that holds one line of the .acl file. (This used to be malloc(sizeof(1024)), which is only
	   4 bytes, so reading a line overflowed the heap.) */
	char* buff = (char*)malloc(1024);
	if (buff == NULL)
	{
		fprintf(stderr, "Out of memory\n");
		return 1;
	}
	FILE *file = fopen ( filename, "r" );
  	if(file == NULL)
    	{
      	printf("Error opening .acl file \n");
	exit(1);
	}
  	/* i counts how many valid lines were read; stop at the size of the arrays so they cannot overflow */
  	int test1, i =0;
  	while(i < MAX_ACL_ENTRIES && fgets(buff, 1024, file) != NULL)
  	{
    		// "%199s" limits the name to what fits in user[i]. A valid line has a name and one permission letter.
    		test1=sscanf(buff, "%199s %c",user[i],&permission[i]);
    		if(test1 == 2)
    		{
      		i++;
    		} else{
     	    		printf("The .acl file has wrong format \n");
		      }
	}
  	/*MAX is the number of entries that were read*/
  	int  MAX = i;
  	fclose(file);
  	free(filename);
  	free(buff);
	int read_fd;
 	int write_fd;
 	struct stat stat_buf;
	// The name of the user who ran this program, looked up from the real user id. (getlogin() was used before;
	// it can return NULL without a terminal and reports the terminal's login, not the real user.)
	struct passwd *pw = getpwuid(ruid);
	if (pw == NULL)
	{
		fprintf(stderr, "Could not find the name of the current user\n");
		return 1;
	}
	char* name = pw->pw_name;
	// Look for the user in the list of allowed names
	for(i=0; i < MAX; i++)
	{
		if (strcmp(user[i], name) == 0)
		{
		// Open the input file.
		read_fd = open (argv[1], O_RDONLY);
		if (read_fd < 0)
		{
			perror("Could not open the source file");
			return 1;
		}
		// Stat the input file to obtain its size.
		if (fstat (read_fd, &stat_buf) < 0)
		{
			perror("Could not read the source file's details");
			close (read_fd);
			return 1;
		}
		/* Open the output file for writing, with the same permissions as the source file. */
		write_fd = open (argv[2], O_WRONLY | O_CREAT, stat_buf.st_mode);
		if (write_fd < 0)
		{
			perror("Could not open the destination file");
			close (read_fd);
			return 1;
		}
		/* Blast the bytes from one file to the other. */
		if (sendfile (write_fd, read_fd, 0, stat_buf.st_size) < 0)
			perror("Copy failed");
		/* Close up. */
		close (read_fd);
		close (write_fd);
		return 0;
		}
	}
printf("The user have no permission to copy the file the proces will be terminated. BYE! \n");
return 0;
}
/* The end of main function */
