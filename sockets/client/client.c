#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char **argv) {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("Failed to create socket\n");
		return 1;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	if (argc < 3) {
		printf("Please supply an IP address and port\n");
		return 1;
	}
	if (inet_aton(argv[1], &addr.sin_addr) < 0) {
		printf("Failed to parse IP address\n");
		return 1;
	};
	int port;
	if ((port = atoi(argv[2])) == 0) {
		printf("Invalid port\n");
		return 1;
	}
	addr.sin_port = htons(port);

	if (connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
		printf("Failed to connect to %s on port %d: %s\n", argv[1], port, strerror(errno));
		return 1;
	}
	
	char *http_req = "CONNECT 127.0.0.1:8081 HTTP/1.1\n\n";
	if (write(sock, http_req, strlen(http_req)) < 0) {
		printf("Failed to write to socket\n");
		return 1;
	}

	char buf[1400];
	int bytes = read(sock, buf, sizeof(buf));
	if (bytes < 0) {
		printf("Failed to read from socket\n");
		return 1;
	} else if (bytes == 0) {
		printf("Connection closed\n");
		return 1;
	} else {
		if (write(1, buf, bytes) < 0) {
			printf("Failed to print response\n");
			return 1;
		}
	}

	char *post_req = "POST /testing HTTP/1.1\n\nLook at this awesome POST request!";
	if (write(sock, post_req, strlen(post_req)) < 0) {
		printf("Failed to send request\n");
		return 1;
	}

	bytes = read(sock, buf, sizeof(buf));
	if (bytes < 0) {
		printf("Failed to read from socket\n");
		return 1;
	} else if (bytes == 0) {
		printf("Connection closed\n");
		return 1;
	} else {
		if (write(1, buf, bytes) < 0) {
			printf("Failed to print response\n");
			return 1;
		}
	}
}
