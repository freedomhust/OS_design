#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h> // for close
 
int main(void)
{
	int fd;
 
	fd = open("/dev/hello",O_RDWR);
	if(fd<0)
	{
		perror("open fail \n");
		return 0;
	}
 
	close(fd);
}
