#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

int main (void)
{
	int fd;
	int numb_read, numb_write;
	char buf1[12] = "hello world\n";

	// assume foobar.txt is already created
	fd = open("hello.txt", O_RDWR | O_CREAT, 0666);
	if (-1 == fd)
	{
		printf("open() hello.txt failed\n");
	}
	
	numb_write = write(fd, buf1, strlen(buf1));
	printf("write %d bytes to hello.txt\n", numb_write);

	lseek(fd, 2, SEEK_SET);
	write(fd, "trungdoan", strlen("trungdoan"));

	close(fd);
	
	return 0;
}

