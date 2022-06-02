#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "socket.h"


int main(int argc, char *argv[])
{
	struct sockaddr srvr_name;
	struct sockaddr rcvr_name;
	int namelen;

	char buf[MAX_MSG_LEN]; 
	int sock;
	int bytes;

	long int curr_time = time(NULL);

	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		perror("Error in socket");
		return ERROR_CREATE_SOCKET;
	}

	srvr_name.sa_family = AF_UNIX;
	strcpy(srvr_name.sa_data, SOCK_NAME);

	if (bind(sock, &srvr_name, LEN_STRUCT_SOCKADDR(srvr_name)) < 0)
	{
		perror("Error in bind");
		return ERROR_BIND_SOCKET;
	}

	while (1)
	{
		bytes = recvfrom(sock, buf, sizeof(buf), 0, &rcvr_name, &namelen);

		if (bytes < 0)
		{
			perror("Error recvfrom");
            close(sock);
	        unlink(SOCK_NAME);
			return ERROR_RECVFROM_SOCKET;
		}

		printf("\nMessage from client: %s\n", buf);
        printf("Message length = %ld\n\n\n", strlen(buf));
	}

    close(sock);
	unlink(SOCK_NAME);

	return OK;
}