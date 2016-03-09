#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <sys/time.h>
#include <linux/ioctl.h>
#include <linux/rtc.h>*/
#include <time.h>
int main(int argc, char **argv)
{
	int fd, res;
	char buff[1024];
	int i = 0;

	if(argc < 2){
		printf("\nGmem_test: A helper program for the driver gmem\nPossible usage is gmem_test [option] <string>\n\n");
		printf("Option\t\t\tDescription\n-----\t\t\t-----------\n");
		printf(
			"-read/show\t\tShow the content of the entire content (256 bytes)\n");
		printf("-write <string>\t\twrite a string to the driver \n\n");

		return -1;
	}

	/* open devices */
	fd = open("/dev/gmem", O_RDWR);
	if (fd < 0 ){
		printf("Can not open device file.\n");		
		return 0;
	}else{
		if(strcmp("show", argv[1]) == 0){
			memset(buff, 0, 1024);
			res = read(fd, buff, 256);
			sleep(1);
			printf("The string read from gmem is '%s'\n", buff);
		}else if(strcmp("write", argv[1]) == 0){
			memset(buff, 0, 1024);
			if(argc >= 3){
				sprintf(buff,"%s", argv[2]);
				for(i = 3; i < argc; i++)
					sprintf(buff,"%s %s",buff,argv[i]);
			}
			printf("The string to be written to gmem is '%s'\n", buff);
			res = write(fd, buff, strlen(buff)+1);
			if(res == strlen(buff)+1){
				printf("Can not write to the device file.\n");		
				return 0;
			}	
		}
		/* close devices */
		close(fd);
	}
	return 0;
}
