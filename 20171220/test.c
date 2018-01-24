#include<sys/types.h>  
#include<sys/stat.h>  
#include<fcntl.h>  
#include<stdio.h> 
#include<string.h>


char buf[] = "123456789";

int main()
{
	int fd = 0 ;
	fd = open("/dev/simple",O_RDWR|O_NONBLOCK);
	if(fd<0)
	{
		printf("open fail！");
	}
	write(fd, buf, strlen(buf));
	close(fd);
	return 0;
}
