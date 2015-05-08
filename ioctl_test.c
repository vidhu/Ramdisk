#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define RD_CREAT _IOR('G', 0, char *)
#define RD_MKDIR _IOR('G', 1, char *)
#define RD_OPEN _IOR('G', 2, char *)
#define RD_CLOSE _IOR('G', 3, int) 
#define RD_READ _IOWR('G', 4, struct Params) //param data structure
#define RD_WRITE _IOWR('G', 5, struct Params) //param ds
#define RD_LSEEK _IOR('G', 6, struct Params) //param ds
#define RD_UNLINK _IOR('G', 7, char *)
#define RD_READDIR _IOWR('G', 8, struct Params) //param ds

#define N 1067008

struct Params {
	int fd;
	char* addr;
	int count;
};

int main(){
    int fd = -1;
    if ((fd = open("/proc/diskos", O_RDONLY)) < 0) {
        perror("open");
        return -1;
    }


    //Create and open file
    ioctl(fd, RD_CREAT, "/file1");
    int rdfd = ioctl(fd, RD_OPEN, "/file1");

    //Write data
    char* msg = malloc(sizeof(char)*N);
    memset(msg, '\0', sizeof(char)*N);
    memset(msg, '1', (sizeof(char)*N)-1);

    struct Params p1 = {
    	.fd = rdfd,
    	.addr = msg,
    	.count = N
    };
    ioctl(fd, RD_WRITE, &p1);
    

    //Close and Delete file
    ioctl(fd, RD_CLOSE, rdfd);
    ioctl(fd, RD_UNLINK, "/file1");



    if ((fd = close(fd)) < 0) {
        perror("close");
        return -1;
    }

    return 0;
}