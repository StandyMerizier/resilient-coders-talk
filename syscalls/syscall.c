#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
	int fd = open("./test.txt", O_RDWR | O_CREAT, 0666);
	char *string = "this is the test text";
	if (write(fd, string, strlen(string)) < 0) {
		printf("ERROR: write() - %s\n", strerror(errno));
	}
	if (unlink("./test.txt") < 0) {
		printf("ERROR: unlink() - %s\n", strerror(errno));
	}
	close(fd);
}
