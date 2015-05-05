#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>

#define RD_CREAT _IOR('G', 0, char *)
#define RD_MKDIR _IOR('G', 1, char *)
#define RD_OPEN _IOR('G', 2, char *)
#define RD_CLOSE _IOR('G', 3, int) 
#define RD_READ _IOWR('G', 4, struct Params) //param data structure
#define RD_WRITE _IOWR('G', 5, struct Params) //param ds
#define RD_LSEEK _IOR('G', 6, struct Params) //param ds
#define RD_UNLINK _IOR('G', 7, char *)
#define RD_READDIR _IOWR('G', 8, struct Params) //param ds

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

    /**
    if(ioctl(fd, RD_CREAT, "/file1") < 0)
        perror("first ioctl");
	**/
	//ioctl(fd, RD_MKDIR, "/dir1");
	//ioctl(fd, RD_MKDIR, "/dir1/dir2");
	//ioctl(fd, RD_CREAT, "/dir1/dir2/file1");
    
    ioctl(fd, RD_CREAT, "/file1");
    ioctl(fd, RD_OPEN, "/file1");

    //char *buf = malloc(sizeof(char)*512);
    char *buf = "Hello World!";
    struct Params p;
    p.fd = 1;
    p.addr = buf;
    p.count = sizeof(char)*512;

    ioctl(fd, RD_WRITE, &p);
    

    return 0;
}