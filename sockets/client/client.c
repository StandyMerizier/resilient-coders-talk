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
	if (inet_aton("127.0.0.1", &addr.sin_addr) < 0) {
		printf("Failed to parse IP address\n");
		return 1;
	};
	int port;
	addr.sin_port = htons(8080);

	if (connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
		printf("Failed to connect to %s on port %d: %s\n", argv[1], port, strerror(errno));
		return 1;
	}
	
	char http_req[1400];
	snprintf(http_req, sizeof(http_req), "CONNECT %s:%s HTTP/1.1\n\n", argv[1], argv[2]);
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

	char get_req[1400];
	snprintf(get_req, sizeof(get_req), "GET / HTTP/1.1\nHost: %s\nUser-Agent: curl/1.61.7\nAccept: */*\n\n", argv[1]);
	if (write(sock, get_req, strlen(get_req)) < 0) {
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
