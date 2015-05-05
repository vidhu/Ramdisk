#ifndef KDISK_H
#define KDISK_H

#define MODULE_NAME "diskos"
#define DISK_SIZE 2

//IOCTL
#define RD_CREAT _IOR('G', 0, char *)
#define RD_MKDIR _IOR('G', 1, char *)
#define RD_OPEN _IOR('G', 2, char *)
#define RD_CLOSE _IOR('G', 3, int) 
#define RD_READ _IOWR('G', 4, struct Params) //param data structure
#define RD_WRITE _IOWR('G', 5, struct Params) //param ds
#define RD_LSEEK _IOR('G', 6, struct Params) //param ds
#define RD_UNLINK _IOR('G', 7, char *)
#define RD_READDIR _IOWR('G', 8, struct Params) //param ds
//End IOCTL

//Macros
#define SET_BIT_1(y) ((0x01 << (7-y)))
#define SET_BIT_0(y) (~SET_BIT_1(y))
//End Macros

//Prototypes
static long procfile_ioctl(struct file *, unsigned int, unsigned long);
int rd_creat(char *pathname);
int rd_mkdir(char *pathname);
int rd_open(char *pathname);
int rd_close(int fd);
int rd_read(int fd, char *address, int num_bytes);
int rd_write(int fd, char *address, int num_bytes);
int rd_lseek(int fd, int offset);
int rd_unlink(char *pathname);
int rd_readdir(int fd, char *address);

char **parse_path(char *pathname);
int get_inode_number(char* pathname);
int get_inode_number_helper(int index, char *dir_name);
int find_free_inode(void);
int insert_Inode(int parent, int child, char *fileName);
union Block* allocate_block(void);
int isBlockFree(int block);
//End prototypes

//Ramdisk constants
#define MAX_FILE_SIZE 	((64*256)+(64*64*256)+256*8)
#define DISK_SIZE 		2	
#define BLOCKSIZE 		256
#define PART_SIZE		(DISK_SIZE * 1024 * 1024) - (BLOCKSIZE * (1+256+4))
#define PART_COUNT		PART_SIZE / BLOCKSIZE

#define INODE_SIZE 		64
#define INODE_BLOCKSPAN 256
#define INODE_COUNT		(BLOCKSIZE * INODE_BLOCKSPAN) / INODE_SIZE
//End Ramdisk constants

//Ramdisk data structures
struct Dir_Entry {
	char filename[14];
	int16_t inode_number;
};

struct Block_dir {
	struct Dir_Entry entry[16];
};

struct Block_file {
	int8_t byte[BLOCKSIZE];
};

union Block {
	struct Block_file file;
	struct Block_dir dir;
};

struct SuperBlock{
	int freeblock;
	int freeInode;
	uint8_t pad[248];
};

struct Inode { //64bytes
	char type[4]; //"dir" or "reg"
	int size;
	union Block *location[10];
	uint8_t pad[16];
};

struct Bitmap {
	uint8_t map[BLOCKSIZE * 4];
};

struct Ramdisk {
	//Super block
	struct SuperBlock superBlock;

	//Index node array (over 256 blocks)
	struct Inode inode[INODE_COUNT];

	//Block Bitmap
	struct Bitmap bitmap;

	//Data Blocks
	union Block part[7931];
};

struct FileDesc {
	int position;
	struct Inode* inode;
};

struct Params {
	int fd;
	char* addr;
	int count;
};
#endif