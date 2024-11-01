#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) // Cấp phát stack frame cho hàm main() 
{

    	pid_t child_pid; 			// Lưu trong stack frame của main() 
    	int counter = 0;
    	int status;
    	child_pid = fork();

    	if (child_pid >= 0)
    	{
        if (0 == child_pid)
        { 
            	//printf("Child process PID = %d\n", getpid());
            	printf("Child process termination\n");
            	// do nothing
        }
        	else
        	{ 
            		//while (1);
			
			wait(&status);
        	}
    	}
    	else
    	{ 
		printf("fork() unsuccessfully\n");
    	}	

    return 0;
}
