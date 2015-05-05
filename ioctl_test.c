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


	ioctl(fd, RD_MKDIR, "/dir1");
    ioctl(fd, RD_MKDIR, "/dir1/dir2_0");
    ioctl(fd, RD_MKDIR, "/dir1/dir2_1");
    ioctl(fd, RD_MKDIR, "/dir1/dir2_2");
    ioctl(fd, RD_MKDIR, "/dir1/dir2_3");   
    ioctl(fd, RD_MKDIR, "/dir1/dir2_4");   
    ioctl(fd, RD_MKDIR, "/dir1/dir2_5");   
    ioctl(fd, RD_MKDIR, "/dir1/dir2_6");
    //ioctl(fd, RD_CREAT, "/dir1/dir2_2/file1");
    
    ioctl(fd, RD_UNLINK, "/dir1/dir2_2");

    int rdfd = ioctl(fd, RD_OPEN, "/dir1");
    char buf[16];
    struct Params write_p;
    write_p.fd = rdfd;
    write_p.addr = buf;
    write_p.count = 16;

    for(int i=0; i<8; i++){
	    if(ioctl(fd, RD_READDIR, &write_p) == 0)
	    	break;
	    char *filename = buf;
	    int16_t inode_num = *(buf + 14);
	    printf("File: %.14s  Inode: %d\n", filename, inode_num);
    }



    //int rdfd = ioctl(fd, RD_OPEN, "/dir1/dir2_2/file1");
    //char buf[] = "Hello World!";
    //struct Params write_p;
    //write_p.fd = rdfd;
    //write_p.addr = buf;
    //write_p.count = sizeof(buf);
    //ioctl(fd, RD_WRITE, &write_p);
    //ioctl(fd, RD_CLOSE, rdfd);
    
	//ioctl(fd, RD_UNLINK, "/dir1/dir2_2/file1");
    //ioctl(fd, RD_CREAT, "/dir1/dir2_2/file1");   
    /**
    char *buf2 = malloc(sizeof(buf));
    struct Params read_p;
    read_p.fd = rdfd;
    read_p.addr = buf2;
    read_p.count = sizeof(buf);
    ioctl(fd, RD_READ, &read_p);
    printf("===> %s <===\n", buf2);

    struct Params lseek;
    lseek.fd = rdfd;
    lseek.count = 3;
    ioctl(fd, RD_LSEEK, &lseek);

    char *buf3 = malloc(sizeof(buf));
    struct Params read_p2;
    read_p2.fd = rdfd;
    read_p2.addr = buf3;
    read_p2.count = 5;
    ioctl(fd, RD_READ, &read_p2);
    printf("===> %s <===\n", buf3);
	**/

    if ((fd = close(fd)) < 0) {
        perror("close");
        return -1;
    }

    return 0;
}