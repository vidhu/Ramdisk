#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include "kdisk.h"

struct proc_dir_entry *proc_file;
struct file_operations proc_file_fops = {
	.owner 			= THIS_MODULE,
	.unlocked_ioctl	= procfile_ioctl,
};

struct FileDesc *fd_table[1024];
struct Ramdisk *disk;

static long procfile_ioctl(struct file *f, unsigned int cmd, unsigned long arg){
	int result;
	int size;
	char *pathname;

	//File ops
	struct Params params;
	int byte_count;
	char *data_address;

	switch(cmd){
		case RD_CREAT:
			size = strnlen_user((char *) arg, 64);
			pathname = (char *)kmalloc(size, GFP_KERNEL);
			copy_from_user(pathname, (char *)arg, size);
			result = rd_creat(pathname);
			break;
		case RD_MKDIR:
			size = strnlen_user((char *) arg, 64);
			pathname = (char *)kmalloc(size, GFP_KERNEL);
			copy_from_user(pathname, (char *)arg, size);
			result = rd_mkdir(pathname);
			break;
		case RD_OPEN:
			size = strnlen_user((char *) arg, 64);
			pathname = (char *)kmalloc(size, GFP_KERNEL);
			copy_from_user(pathname, (char *)arg, size);
			result = rd_open(pathname);
			break;
		case RD_CLOSE:
			result = rd_close((int) arg);
			break;
		case RD_READ:
			//Get params
			copy_from_user(&params, (struct Params*)arg, sizeof(struct Params));

			//Allocate space to read data
			data_address = (char *) kmalloc(params.count, GFP_KERNEL);

			//Read data
			byte_count = rd_read(params.fd, data_address, params.count);

			//Check if successfull
			if(params.count != byte_count){
				result = -1;
				break;
			}

			//Copy read data to user space
			copy_to_user(params.addr, data_address, params.count);
			printk("RD_Read successfull\n");
			break;
		case RD_WRITE:
			//Get params
			copy_from_user(&params, (struct Params*)arg, sizeof(struct Params));

			//Get Data to write
			data_address = (char *) kmalloc(params.count, GFP_KERNEL);
			copy_from_user(data_address, params.addr, params.count);

			//Write data
			byte_count = rd_write(params.fd, data_address, params.count);
			
			//Check if successful
			//Check if successfull
			if(params.count != byte_count){
				result = -1;
				break;
			}
			printk("RD_Write successfull\n");

			break;
		default:
			return -ENOTTY;
	}
	return result;
}

int rd_creat(char *pathname){
	char **data = parse_path(pathname);
	char *dirname = data[0];
	char *parentdir = data[1];
	printk("==MK_CREAT=========================\n");
	printk("Creating file: %s\n", pathname);
	printk("File name: '%s'\tDir name: '%s'\n", dirname, parentdir);

	//Get end path inode
	int parent_inode = get_inode_number(parentdir);
	printk("Dir inode at: %d\n", parent_inode);

	//Create the file
	int newInode = find_free_inode();
	strncpy(disk->inode[newInode].type, "reg", 4);
	disk->inode[newInode].size = 0;
	printk("File inode at: %d\n", newInode);


	//Insert new Inode into parent's
	insert_Inode(parent_inode, newInode, dirname);
	//Debug: check inode number of newly create dir "/dir1"
	
	printk("'Testing: /%s' at: %d\n", dirname, get_inode_number(pathname));
	printk("=================================\n");
	return 0;
}

int rd_mkdir(char *pathname){	
	char **data = parse_path(pathname);
	char *dirname = data[0];
	char *parentdir = data[1];
	printk("==MK_DIR=========================\n");
	printk("Creating dir: %s\n", pathname);
	printk("Dir name: '%s'\tParent name: '%s'\n", dirname, parentdir);

	//Get end path inode
	int parent_inode = get_inode_number(parentdir);
	printk("Parent inode at: %d\n", parent_inode);

	//Create the directory
	int newInode = find_free_inode();
	strncpy(disk->inode[newInode].type, "dir", 4);
	disk->inode[newInode].size = 0;
	printk("Child inode at: %d\n", newInode);


	//Insert new Inode into parent's
	insert_Inode(parent_inode, newInode, dirname);
	//Debug: check inode number of newly create dir "/dir1"
	
	printk("'Testing: /%s' at: %d\n", dirname, get_inode_number(pathname));
	printk("=================================\n");
	return 0;
}


int rd_open(char *pathname){
	printk("==RD_OPEN========================\n");

	//Get Inode index for the path name
	int inode_index = get_inode_number(pathname);
	printk("File is at inode: %d\n", inode_index);

	//Returns error if path doesn't exist
	if(inode_index  == -1)
		return -1;

	//Some other process is using it. lol
	if(fd_table[inode_index] != NULL)
		return -1;

	//Create new file Descriptor
	struct FileDesc* fd = (struct FileDesc*) kmalloc(sizeof(struct FileDesc), GFP_KERNEL);
	fd->read_pos = 0;
	fd->write_pos = 0;
	fd->inode = &(disk->inode[inode_index]);

	//Insert file descriptor into fd table
	fd_table[inode_index] = fd;
	printk("fd for inode '%d' is pointing to file_desc with addr:0x%p\n", inode_index, fd_table[inode_index]);

	printk("=================================\n");

	return inode_index;
}

int rd_close(int fd){
	printk("==RD_CLOSE=======================\n");
	
	printk("int fd = %d\n", fd);
	if(fd_table[fd] == NULL)
		return -1;

	//Free memory
	printk("Freeing memory at: 0x%p\n", fd_table[fd]);
	kfree(fd_table[fd]);

	//Remove pointer from file descriptor table
	printk("Setting fd_table[%d] = NULL\n", fd);
	fd_table[fd] = NULL;

	printk("=================================\n");
	return 0;
}

int rd_read(int fd, char *address, int num_bytes){
	printk("==RD_READ========================\n");
	printk("Need to read '%d bytes' from inode '%d' into address: 0x%p\n", num_bytes, fd, address);
	
	int bytes_read = 0;

	struct Inode *inode = &disk->inode[fd];
	
	for(int i=0; i<8; i++){
		//Check if block is allocated or not. If not, then there is no content
		if(inode->location[i] == NULL)
			return -1;

		union Block *b = inode->location[i];
		printk("Reading data from inode.location[%d] at addr: %p\n", i, b);
		for(int j=0; j<256; j++){
			printk("\tReading char: %c\n", b->file.byte[j]);
			address[(256*i)+j] = b->file.byte[j];
			bytes_read++;

			if(bytes_read == num_bytes){
				printk("Bytes read: %d\n", bytes_read);
				printk("=================================\n");
				return bytes_read;
			}
		}
	}
	
	return -1;
}

int rd_write(int fd, char *address, int num_bytes){
	printk("==RD_WRITE=======================\n");
	printk("Need to write '%d bytes' into inode '%d' from address: 0x%p\n", num_bytes, fd, address);
	printk("Address '0x%p' contains: %s\n", address, address);

	int bytes_written = 0;

	struct Inode *inode = &disk->inode[fd];
	for(int i=0; i<8; i++){
		//Check if block is allocated or not. If not, allocate it a block
		if(inode->location[i] == NULL)
			inode->location[i] = allocate_block();

		printk("Writting data to inode.location[%d] at addr: %p\n", i, inode->location[i]);
		for(int j=0; j<256; j++){
			printk("\tWritting char: %c\n", address[(256*i)+j]);
			inode->location[i]->file.byte[j] = address[(256*i)+j];
			bytes_written++;

			if(bytes_written == num_bytes){
				printk("Bytes Written: %d\n", bytes_written);
				printk("=================================\n");
				return bytes_written;
			}
		}
	}

	return -1;
}

//Returns an array of 2 string containing the parent directory
//and end file. Example: char *pathname = "/dir1/dir2/file1"
//will return; parent: `/dir1/dir2`, child: `file1`
char **parse_path(char *pathname){
	//Get new directory name
	char *dirname = (strrchr(pathname, '/')) +1;

	//Trim to get parent
	int size = strlen(pathname)-strlen(dirname)+1;
	char *parent = kmalloc(size, GFP_KERNEL);
	memset(parent, '\0', size);
	strncpy(parent, pathname, size-2);
	if(parent[0] == '\0')
		parent[0] = '/';

	char **data = kmalloc(2*sizeof(char*), GFP_KERNEL);
	data[0] = dirname;
	data[1] = parent;

	return data;
}

//Scans the inode blocks to find a free inode
//Return the index of the free inode it found
int find_free_inode(){
	//Check if all inodes are full or not
	if(disk->superBlock.freeInode == 0)
		return -1;

	for(int i=0; i < INODE_COUNT; i++){
		if(disk->inode[i].type[0] == 0){ //inod is free
			disk->superBlock.freeInode--;
			return i;
		}
	}

	return -1;
}

//Returns the inode index of the provided path
int get_inode_number(char* pathname){
	if(pathname[0] != '/')
		return -1;
	if(strlen(pathname) == 1)
		return 0;

	pathname++;

	//Get inode index of path be going through the path name
	int nodeIndex = 0; //inode of root dir
  	char *token, *strpos = pathname;
  	while ((token=strsep(&strpos,"/")) != NULL){
    	nodeIndex = get_inode_number_helper(nodeIndex, token);
    	printk("Node Index for '%s': %d\n", pathname ,nodeIndex);
  	}
  	

  	return nodeIndex;
}

//returns the inode of dir_name if it is present in inode
//of the given index
int get_inode_number_helper(int index, char *dir_name){
	printk("Need to look for '%s' in inode '%d'\n", dir_name, index);
	if(dir_name[0] == '/')
		return 0;

	struct Inode *inode = &(disk->inode[index]);
	
	for(int i=0; i<8; i++){
		union Block *b = inode->location[i];

		//Search for entires in the block
		if(b != 0){
			for(int j=0; j<16; j++){
				//Some entry in this...?
				if(b->dir.entry[j].filename[0] != 0){
					if(!strncmp(b->dir.entry[j].filename, dir_name, 14))
						return b->dir.entry[j].inode_number;
				}
			}
		}
	}
	
	return -1;
}

//Inserts an child inode under a parent inode. For example:
//if 	parent inode = 123 which links to (/dir1/dir2/dir3)
//if 	child  		 = 124 which links to (file1)
//then the following struction is formed /dir1/dir2/dir3/file1
int insert_Inode(int parent, int child, char *fileName){
	printk("Inserting '%s' at inode %d in parent with inode %d\n", fileName, child, parent);

	union Block *b;
	for(int i=0;i<8;i++){
		b = disk->inode[parent].location[i];
		//Allocate new block
		if(b == 0){
			disk->inode[parent].location[i] = allocate_block();
			b = disk->inode[parent].location[i];
		}
		for(int j=0; j<16; j++){
			if(b->dir.entry[j].filename[0] == 0){ //found a free area
				strncpy(b->dir.entry[j].filename, fileName, 13); //truncate any long strings
				b->dir.entry[j].inode_number = child;
				disk->superBlock.freeInode--;
				return 0;
			}
		}
	}	

	return -1;
}

//allocates a free block in the disk.
//Returns the address of the newly allocated block
union Block* allocate_block(){
	for(int i=0; i<7931; i++){
		if(isBlockFree(i)){
			disk->bitmap.map[i / 8] = (disk->bitmap.map[i / 8] | SET_BIT_1(i%8));
			disk->superBlock.freeblock--;
			return &disk->part[i];
		}
	}
	return NULL;
}

//Uses the bitmap to check if a block is free is not
int isBlockFree(int block){
	uint8_t *bitmap = &(disk->bitmap.map[block / 8]);
	return ((*bitmap >> (7-block%8)) & 0x1) == 0;
}

int init_module(){
	//Create process file
	proc_file = proc_create(MODULE_NAME, 0, NULL, &proc_file_fops);

	//Initialize Ramdisk
	disk = (struct Ramdisk*) vmalloc(sizeof(struct Ramdisk));
	memset(disk, 0, sizeof(struct Ramdisk));
	
	//Init Inode
	memcpy(disk->inode[0].type, "dir", 4);
	disk->inode[0].size = 0;

	//Init supper block
	disk->superBlock.freeInode = INODE_COUNT - 1; //Account for root
	disk->superBlock.freeblock = PART_COUNT;

	printk("Ram disk Initialized: %s\n", MODULE_NAME);
	return 0;
}

void cleanup_module(){
	remove_proc_entry(MODULE_NAME, NULL);
	printk("Unloaded %s\n", MODULE_NAME);
}
