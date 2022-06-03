#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

static const char* socket_path = "socket.soc";
static const unsigned int nIncomingConnections = 10;

int main()
{
	int s = 0;
	int s2 = 0;
	struct sockaddr_un local, remote;
	int len = 0;

	s = socket(AF_UNIX, SOCK_STREAM, 0);
	if  (s == -1)
	{
		printf("socket error \n");
		return 1;
	}

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, socket_path);
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);
	if (bind(s, (struct sockaddr*)&local, len) != 0)
	{
		printf("bind error \n");
		return 1;
	}

	if (listen(s, nIncomingConnections) != 0 )
	{
		printf("Error on listen call \n");
	}

	while (1)
	{
        struct sockaddr_un new;
		unsigned int sock_len = 0;
        
		if( (s2 = accept(s, (struct sockaddr*)&new, &sock_len)) == -1 )
		{
			printf("accept error \n");
			return 1;
		}

		int data_recv = 0;
		char recv_buf[100];
		char send_buf[200];
		do
        {
			memset(recv_buf, 0, 100*sizeof(char));
			memset(send_buf, 0, 200*sizeof(char));
			data_recv = recv(s2, recv_buf, 100, 0);
			if (data_recv > 0)
			{
				printf("Received messages from: %s \n", recv_buf);
				strcpy(send_buf, "+ From server: ");
				strcat(send_buf, recv_buf);

				if (send(s2, send_buf, strlen(send_buf)*sizeof(char), 0) == -1)
				{
					printf("send error \n");
				}
			}
			else
			{
				printf("error recv \n");
			}
		} while(data_recv > 0);

		close(s2);
	}


	return 0;
}