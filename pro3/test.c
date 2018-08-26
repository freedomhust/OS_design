#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_SIZE 1024

int main()
{
	int fd;
	char buf[MAX_SIZE]; //缓冲区
	char get[MAX_SIZE]; //要写入的信息

	/*获取设备名*/
	char devName[20];
	char dir[50] = "/dev/";
	system("ls /dev/");
	printf("Please input the device's name: ");
	gets(devName);
	strcat(dir, devName);
	fd = open(dir, O_RDWR | O_NONBLOCK);//可读写
	//O_NONBLOCK :路径名指向字符文件，把文件的打开和后继 I/O 
	//设置为非阻塞模式（nonblocking mode）
	if( fd != -1)
	{
		//读初始信息
		read( fd , buf , sizeof(buf) );
		printf( "%s\n" , buf);

              	//写信息
		printf( "Please input a string : ");
		gets(get);
		write( fd , get , sizeof(get) );

		//读刚才写的信息
		read(fd, buf, sizeof(buf));
		printf("device Message : %s\n", buf);

		close(fd);
		return 0;
	}
	else
	{
		printf("Device open failed !\n");
		return -1;
	}

}