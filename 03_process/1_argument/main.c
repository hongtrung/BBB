#include<stdio.h>
#include<stdlib.h>

void main(int argc, char* argv[])
{
	int i;
	// số lượng command-line truyền vào
	printf("Number of argument: %d \n",argc);
	// in ra nội dung của mỗi command-line
	for(i = 0; i < argc; i++)
	{
		printf("argc[%d]: %s\n", i+1, argv[i]);
	}
}
