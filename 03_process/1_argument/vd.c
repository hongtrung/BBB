#include<stdio.h>
#include<stdlib.h>

char buff[1024];		// uninitialized data segment
int primes[] = {2, 4, 5, 7};	// initialized data segment

void hello(int x)		// cấp phát stack frame cho hàm hello()
{
	int result;		// uninitialized data segment
}

void main(int argc, char* argv[])	// cấp phát stack frame cho hàm main()
{
	static int key = 1;		// initialized data segment
	static char buff[1024];		// uninitialized data segment
	char *p;			// stack frame của main()
	p = malloc(1024);		// trỏ tới bộ nhớ được cấp phát ở Heap segment
}

