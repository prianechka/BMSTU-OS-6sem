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
	int sock;
    int bytes;
    int namelen;

	char buf[MAX_MSG_LEN];
    long int curr_time = time(NULL);

	sprintf(buf, "\nProcess id = %d \nSended at time: %sMessage: ", getpid(), ctime(&curr_time));

    if (argc < 2)
		strcat(buf, "Hello server!");
	else
		strcat(buf, argv[1]);

	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		perror("Error in function: socket");
		return ERROR_CREATE_SOCKET;
	}

    srvr_name.sa_family = AF_UNIX;
	strcpy(srvr_name.sa_data, SOCK_NAME);

	if (sendto(sock, buf, strlen(buf) + 1, 0, &srvr_name, LEN_STRUCT_SOCKADDR(srvr_name)) < 0)
	{
		perror("Error in SendTO");
		return ERROR_SENDTO_SOCKET;
	}
    printf("Message is sent to server!\n");

	close(sock);
	return OK;
}