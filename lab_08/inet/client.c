#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#include "constants.h"

int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);

	struct sockaddr_in serv_addr =
		{
			.sin_family = AF_INET,
			.sin_addr.s_addr = INADDR_ANY,
			.sin_port = htons(PORT)};

	char buf[MAX_LEN_BUF];
	int sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (socket < 0)
	{
		perror("Error in socket");
		return ERROR_CREATE_SOCKET;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Error in connect: %d", errno);
		return ERROR_BIND_SOCKET;
	}

	while (TRUE)
	{

		scanf("%s", buf);

		if (sendto(sock, buf, strlen(buf) + 1, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		{
			perror("Error in sentTo");
			return ERROR_SENDTO_SOCKET;
		}

		printf("Message is sent!\n");
	}

	close(sock);

	return OK;
}

//
// fcntl() + флаг O_NONBLOCK делает его неблокирующим.
//
// Сокеты бывают блокирующие и неблокирующие.
// В случае блокирующих сокетов при попытке прочитать(и записать)
// данные функция чтения будет ждать до тех пор, пока не прочитает
// хотя бы один байт или произойдет разрыв соединения или придет сигнал.
// В случае неблокирующих сокетов функция чтения проверяет,
// есть ли данные в буфере, и если есть - сразу возвращает,
// если нет, то она не ждет и также сразу возвращает, что прочитано 0 байт.
//
// Теперь любой вызов функции read() для сокета sock будет возвращать управление сразу же.
// Если на входе сокета нет данных для чтения, функция read() вернет значение EAGAIN.
// sock = socket(PF_INET, SOCK_STREAM, 0);
// fcntl(sock, F_SETFL, O_NONBLOCK);