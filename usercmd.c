#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"

int main () {

	int rc;
	//char *path = "/home";
	//rc = rd_mkdir(path);
	//printf("rc: %d\n", rc);
	char *path2 = "/file";
	rc = rd_creat(path2);
	printf("rc: %d\n", rc);
	int fd = rd_open(path2);
	printf("fd: %d\n", fd);
	char *content = "hello im just testing this out...";
	rc = rd_write(fd, content, strlen(content)+1);
	printf("rc: %d\n", rc);
	char *words;
	words = (char *)malloc(strlen(content)+1);
	//rc = rd_read(fd, words, strlen(content)+1);
	//printf("this was read: %s\n", words);
	rc = rd_close(fd);
	printf("rc: %d\n", rc);
	rc = rd_unlink("path2");
	printf("rc: %d\n", rc);
}

