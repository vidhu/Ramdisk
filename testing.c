#include <stdio.h>
#include <string.h>

int main(){
	get_inode_number("/dir1");
	

	return 0;
}

int get_inode_number(char* pathname){
	//Trim to get parent
	char *tmp = (strrchr(pathname, '/')) + 1;
	memset(tmp, '\n', strlen(tmp));
	printf("Debug: %s", pathname);
	return 0;
}

/**
	//Sanity check
	if (pathname[0] != '/' && strlen(pathname) > 1)
		return -1;
	pathname++; //remove '/' at the start

	//Get inode index of path
	int nodeIndex = 0; //inode of root dir
  	char *token, *strpos = pathname;
	int i = 0;
  	while ((token=strsep(&strpos,"/")) != NULL){  		
    	nodeIndex = get_inode_number_helper(nodeIndex, token);
    	i++;
    	printk("%s\t%d\n", token, i);
  	}
  	if(nodeIndex == -1)
  		return -1;
  	printk("Node Index is: %d\n", nodeIndex);

  	return nodeIndex;
**/