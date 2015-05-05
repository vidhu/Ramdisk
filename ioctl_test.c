#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    
    char *buf = malloc(260);
    memset(buf, '\0', 260);
    memset(buf, 'a', 259);

    struct Params write_p;
    write_p.fd = 1;
    write_p.addr = buf;
    write_p.count = 260;
    ioctl(fd, RD_WRITE, &write_p);
    
    char *buf2 = malloc(260);
    struct Params read_p;
    read_p.fd = 1;
    read_p.addr = buf2;
    read_p.count = 260;
    ioctl(fd, RD_READ, &read_p);

    printf("===> %s\n <===", buf2);

    return 0;
}