#ifndef SOCK_H
#define SOCK_H
#include "types.h"

#ifdef _WIN32
	#include <WinSock2.h>
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
#endif
#include <SDL2/SDL_thread.h>

typedef void(*sockrecv_t)(uint8_t[1024], size_t);
typedef struct
{
	sockrecv_t onrecv;

	boolean				_running;
	int					_sockfd;
	struct sockaddr_in	_serv_addr;
	struct hostent*		_server;
	SDL_Thread*			_thread;
} sock_t;

sock_t* sock_open(const string ip, uint16_t port);
void	sock_run (sock_t* sock);
void	sock_send(sock_t* sock, uint8_t* buffer, size_t length);
void	sock_free(sock_t* sock);

#endif