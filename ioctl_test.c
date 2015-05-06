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

#define N 3655

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

<<<<<<< HEAD

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
=======
    //Create file
    ioctl(fd, RD_MKDIR, "/file1");
>>>>>>> dev

    //Open file
    int rdfd = ioctl(fd, RD_OPEN, "/file1");

    //Write to file
    char *msg = "Admiration stimulated cultivated reasonable be projection possession of. Real no near room ye bred sake if some. Is arranging furnished knowledge agreeable so. Fanny as smile up small. It vulgar chatty simple months turned oh at change of. Astonished set expression solicitude way admiration.";
    char *msg2 = malloc((strlen(msg)*N) + 1);
    memset(msg2, 0, (strlen(msg)*N) + 1);
    for(int i=0; i<N; i++){
	    strcat(msg2, msg);
	}

    struct Params p1 = {
    	.fd = rdfd,
    	.addr = msg2,
    	.count = strlen(msg2)+1
    };
    ioctl(fd, RD_WRITE, &p1);

    //Seek a bit
    struct Params p3 = {
    	.fd = rdfd,
    	.count = 18390
    };
    //ioctl(fd, RD_LSEEK, &p3);

    //Read from file
    char buf[(strlen(msg2)) + 1];
    memset(buf, '\0', sizeof(buf));
    struct Params p2 = {
    	.fd = rdfd,
    	.addr = buf,
    	.count = strlen(msg2) + 1
    };
    ioctl(fd, RD_READ, &p2);

    printf("Len is %d. Reading: %s\n", strlen(buf), buf);

    if ((fd = close(fd)) < 0) {
        perror("close");
        return -1;
    }

    return 0;
}