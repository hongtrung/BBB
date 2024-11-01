#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char const* argv[])	// cấp phát stack frame cho hàm main()
{
	pid_t child_pid;	// lưu trong stack frame của main()
	int counter = 2;	// lưu trong frame của main() 

	printf("gia tri khoi tao cua counter: %d\n", counter);
	
	child_pid = fork();
	if(child_pid >= 0)	//process con
	{
		if(0 == child_pid)
		{
			printf("\n Im the child process, counter: %d\n", ++counter);
			printf("My PID is: %d, my parent PID is: %d\n", getpid(), getppid());
			//sleep(1);
		}
	
		else			//process cha
		{
			printf("Im the parent process, counter: %d\n", ++counter);
			printf("My PID is: %d\n", getpid());
			while(1);
		}
	}else
	{
		printf("fork() unsuccessfully\n");	//fork() return -1 bị lỗi
	}

	return 0;
}
