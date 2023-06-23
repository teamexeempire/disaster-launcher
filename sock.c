#include "sock.h"
#include "log.h"

void sock_mainloop(sock_t* sock)
{
	if (connect(sock->_sockfd, (struct sockaddr*)&sock->_serv_addr, sizeof(sock->_serv_addr)) < 0)
	{
		ILOG("Socket failed to connect");
		return;
	}

	while (sock->_running)
	{
		uint8_t buffer[64];

		int n = recv(sock->_sockfd, (char*)buffer, 64, MSG_WAITALL);
		if (n < 0) //error or closed
		{
#ifdef _WIN32
			closesocket(sock->_sockfd);
#else
			close(sock->_sockfd);
#endif
			sock->_running = FALSE;
			continue;
		}

		if(n == 0)
		{
			SDL_Delay(5);
			continue;
		}
		
		ILOG("Socket got %d bytes of information.", n);
		if(sock->onrecv)
			sock->onrecv(buffer, n);
	}

	ILOG("Socket exited main loop");
}

sock_t* sock_open(const string ip, uint16_t port)
{
	sock_t* sock = malloc(sizeof(sock_t));

	if (!sock)
		return NULL;

	memset(sock, 0, sizeof(sock_t));

#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		free(sock);
		return NULL;
	}
#endif

	sock->_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sock->_sockfd < 0)
	{
		free(sock);
		return NULL;
	}

	sock->_server = gethostbyname(ip);
	memset(&sock->_serv_addr, 0, sizeof(sock->_serv_addr));
	memcpy(&sock->_serv_addr.sin_addr.s_addr, sock->_server->h_addr, sock->_server->h_length);

	sock->_serv_addr.sin_family = AF_INET;
	sock->_serv_addr.sin_port = htons(port);

	ILOG("Socket 0x%p created", (void*)sock);
	return sock;
}

void sock_run(sock_t* sock)
{
	sock->_running = TRUE;
	sock->_thread = SDL_CreateThread((SDL_ThreadFunction)sock_mainloop, "sock_thread", sock);
}

void sock_free(sock_t* sock)
{
	sock->_running = FALSE;
	SDL_WaitThread(sock->_thread, NULL);
}
