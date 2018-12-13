/* To compile the code type "gcc time.c -o time -lrt" 
The last argument is required to link the code with libraries that contain shm_open() etc */
//libraries for i/o, string and malloc/exit functions
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//libraries for time , fork and wait functions
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>
//libraries for creating shared memory objects 
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
//function to convert a long integer to a string
char* integer_to_string(long int x)
{
	//length required for the string is found and memory is allocated accordingly
	int length = snprintf( NULL, 0, "%ld", x );
	char* str =  (char*)malloc(length + 1);
	
	//The number is now loaded as a string into the str variable
	snprintf( str, length + 1, "%ld", x );
	return str;
}
//function to convert a string to a long integer
long int string_to_integer(char* s)
{
	long int x;
	//sscanf() is an inbuilt function that loads the integer in the variable's address 
	sscanf(s,"%ld",&x);
	return x;
}
//global constants in the code
//max size of memory mapped file is 4096 bytes 
const int SIZE = 4096;
//name of the memory mapped file
const char* name = "Shared_memory";
/* argc --> number of arguments in the commandline
   argv --> array of strings (including ./a.out) */
int main(int argc,char** argv)
{
	if(argc == 1)
	{
		printf("A linux command has to be specified after the executable name\n");
		return 0; 
	}	
	// temp contains the inputted array of strings except for the first one i.e the executable name.
	char* temp[argc-1];
	for(int i=1;i<argc;i++)
	{
		temp[i-1]=argv[i];
	}
	temp[argc-1]=NULL;
	// shm_open() system call creates a shared memory object and returns a filedescriptor associated with the object 
	// O_CREAT -- create a new object by the given name if it didn't exist before
	// O_RDWR  -- Read , Write privileges
	// O_TRUNC -- If the file already exists , remove its contents 
	int fd = shm_open(name,O_CREAT | O_RDWR | O_TRUNC,0666);
	//setting size of the object
	ftruncate(fd,SIZE);
	//fork() system call
	pid_t id = fork();
	//fork failure
	if(id == -1)
	{
		printf("fork error\n");
		exit(0);
	}
	//child process
	else if( id == 0)
	{
		//child starts the timer
		struct timeval start;
		gettimeofday(&start,NULL);
		// mmap() Establishes the memory mapped file 
		char* ptr = (char*) mmap(0,SIZE,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
		if(ptr == MAP_FAILED)
		{
			// memory mapping failure
			printf("mmap error\n");
			exit(0);
		}
		// the start time in "second,microsecond" format is written into the file as a string . The format is achieved by concatenation
		// strcat(a,b) => a = a + b
		char* str1 = integer_to_string(start.tv_sec);
		strcat(str1,",");
		char* str2 = integer_to_string(start.tv_usec);
		strcat(str1,str2);
		sprintf(ptr,"%s",str1);
		// execvp takes in the executable name and the actual command strings and runs it
		// This only works for binaries in /bin/ ( and other locations in $PATH ) . Therefore  this wouldnt work on shell_builtin commands like cd,history etc
		if( execvp(temp[0],temp) == -1 )
		{
			//exec failure
			printf("exec error\n");
			exit(0);	
		}
	}
	//parent process
	else
	{
		//wait for the child process to end
		wait(NULL);
		//Record the ending time
		struct timeval end;
		gettimeofday(&end,NULL);
		//map the shared memory object and read the string in it
		char* ptr = (char*) mmap(0,SIZE,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
		if(ptr == MAP_FAILED)
		{
			//memory mapping failure
			printf("mmap error\n");
			exit(0);
		}
		//split the string wrt comma into left and right which gives start time in seconds and microseconds respectively
		char* left = strtok(ptr,",");
		char* right = strtok(NULL,",");
		long int start_sec = string_to_integer(left);
		long int start_micro_sec = string_to_integer(right);
		//unmapping memory and then unlinking the shared object
		munmap(ptr,SIZE);
		shm_unlink(name);
		//time difference in seconds with accuracy in micro seconds
		double time = ( end.tv_sec - start_sec ) +  ( end.tv_usec - start_micro_sec )/1000000.0;
		printf("Elapsed time : %lf \n",time);
	}
	return 0;
}