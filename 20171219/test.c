#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>


int main()
{
	int fd = 0 ;
	fd = open("/dev/simple",O_RDONLY|O_NONBLOCK);
	if(fd<0)
	{
		printf("%d===\n",fd);
		printf("打开fail！");
	}
	return 0;
}
