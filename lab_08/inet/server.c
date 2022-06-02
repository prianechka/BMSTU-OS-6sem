#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>

#include "constants.h"

#define MAX_COUNT_SOCK 10

int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);

	int arr_sock[MAX_COUNT_SOCK] = {0};
	struct sockaddr_in serv_addr =
		{
			.sin_family = AF_INET,
			.sin_addr.s_addr = INADDR_ANY,
			.sin_port = htons(PORT)};

	int clen;
	struct sockaddr_in cli_addr;

	fd_set set; 
	int sock, max_sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	max_sock = sock;

	if (socket < 0)
	{
		printf("Error in socket: %d\n", errno);
		return ERROR_CREATE_SOCKET;
	}

	if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Error in bind: %d\n", errno);
		return ERROR_BIND_SOCKET;
	}

	if (listen(sock, 1) < 0)
	{
		printf("Error in listen: %d\n", errno);
		return ERROR_LISTEN_SOCKET;
	}

	printf("Server is work!\n");

	while (TRUE)
	{
		FD_ZERO(&set);
		FD_SET(sock, &set);

		max_sock = sock;
		for (int i = 0; i < MAX_COUNT_SOCK; i++)
		{
			if (arr_sock[i] > 0)
			{
				FD_SET(arr_sock[i], &set);
				max_sock = arr_sock[i] > max_sock ? arr_sock[i] : max_sock;
			}
		}

		int retval = select(max_sock + 1, &set, NULL, NULL, NULL);
		if (retval < 0)
		{
			printf("Error in select: %d\n", errno);
			return ERROR_SELECT_SOCKET;
		}

		if (FD_ISSET(sock, &set))
		{
			printf("New connection.\n");

			int newsock = accept(sock, NULL, NULL);
			if (newsock < 0)
			{
				printf("accept() failed: %d\n", errno);
				return ERROR_ACCEPT_SOCKET;
			}

			int flag = 1;
			for (int i = 0; i < MAX_COUNT_SOCK && flag; i++)
			{
				if (arr_sock[i] == 0)
				{
					arr_sock[i] = newsock;
					printf("Client number %d\n", i);
					flag = 0;
				}
			}
			if (flag)
			{
				printf("Limit of clients\n");
			}
		}
		for (int i = 0; i < MAX_COUNT_SOCK; i++)
		{
			if (arr_sock[i] && FD_ISSET(arr_sock[i], &set))
			{

				char buf[MAX_LEN_BUF];

				int rv = recvfrom(arr_sock[i], buf, sizeof(buf), 0, NULL, NULL);
				if (rv == 0)
				{
					printf("Close connection with client %d\n", i);
					close(arr_sock[i]);
					arr_sock[i] = 0;
				}
				else
				{
					printf("Client number %d \n Message: %s\n", i, buf);
				}
			}
		}
	}

	close(sock);
	return OK;
}