#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

static const char* socket_path = "socket.soc";
static const unsigned int s_recv_len = 200;
static const unsigned int s_send_len = 100;

int main()
{
	int sock = 0;
	int data_len = 0;
	struct sockaddr_un remote;
	char recv_msg[s_recv_len];
	char send_msg[s_send_len];
	char msg[s_send_len];

	memset(recv_msg, 0, s_recv_len*sizeof(char));
	memset(send_msg, 0, s_send_len*sizeof(char));

	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1  )
	{
		printf("error socket \n");
		return 1;
	}

	remote.sun_family = AF_UNIX;
	strcpy( remote.sun_path, socket_path);
	data_len = strlen(remote.sun_path) + sizeof(remote.sun_family);

	if (connect(sock, (struct sockaddr*)&remote, data_len) == -1)
	{
		printf("error in connect \n");
		return 1;
	}

	printf("Client: Connected \n");
	sprintf(msg, "client pid = %d", getpid());

	if (send(sock, msg, strlen(msg)*sizeof(char), 0 ) == -1)
	{
		printf("send error \n");
	}

	memset(send_msg, 0, s_send_len*sizeof(char));
	memset(recv_msg, 0, s_recv_len*sizeof(char));

	if ((data_len = recv(sock, recv_msg, s_recv_len, 0)) > 0)
	{
		printf("%s \n", recv_msg);
	}
	else
	{
		if (data_len < 0)
		{
			printf("recv error \n");
		}
		else
		{
			printf("socket is closed \n");
			close(sock);
		}
	}

	return 0;
}