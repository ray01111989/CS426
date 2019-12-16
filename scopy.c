#include "functions.h"

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
	/* Here I asing tow demantion array and another array for the permission */
	char user[200][200], permission[200];
	char* filename = (char*)malloc(strlen(argv[1]) * sizeof(char) + strlen(".acl") + 1);
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
	/* allocated some momery and oping a file for read */
	char* buff = (char*)malloc(sizeof(1024));
	FILE *file = fopen ( filename, "r" );
  	if(file == NULL)
    	{	
      	printf("Error opening .acl file \n");
	exit(1);
	}
  	/* initializing some varbiles for the loops */
  	int test1, i =0;
  //	int i = 0;
  	while(fgets(buff, 1024, file) != NULL)
  	{
    		test1=sscanf(buff, "%s %c",user[i],&permission[i]);
    		if(test1 == 2)
    		{
      		i++;
    		} else{
     	    		printf("The .acl file has wrong format \n");
		      }
	}
  	/*Set the i count to MAX for the for loop*/
  	int  MAX = i;
  	fclose(file);
  	free(filename);
  	free(buff);
	int read_fd;
 	int write_fd;
 	struct stat stat_buf;
//	char *name;
	char* name = getlogin();
	for(i=0; i < MAX; i++)
	{
		if (strcmp(user[i], name) == 0)
		{
		// Open the input file.
		read_fd = open (argv[1], O_RDONLY);
		// Stat the input file to obtain its size.
		fstat (read_fd, &stat_buf);
		/* Open the output file for writing, with the same permissions as the source file. */
		write_fd = open (argv[2], O_WRONLY | O_CREAT, stat_buf.st_mode);
		/* Blast the bytes from one file to the other. */
		sendfile (write_fd, read_fd, 0, stat_buf.st_size);
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
