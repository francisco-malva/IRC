/*=========================== Servidor Basico TCP ===============================
Este cliente destina-se a enviar mensagens passadas na linha de comando, sob
a forma de um argumento, para um servidor especifico cuja locacao e' dada
pelas seguintes constantes: SERV_HOST_ADDR (endereco IP) e SERV_UDP_PORT (porto)

O protocolo usado e' o UDP.
==============================================================================*/

#include <winsock.h>
#include <stdio.h>
#include <stdint.h>

#include "../Common/args.h"
#include "../Ex9_Comum/message.h"

#pragma comment (lib, "Ws2_32.lib")

#define SERV_HOST_ADDR "127.0.0.1"
#define SERV_TCP_PORT  6000

#define BUFFERSIZE     4096

#define TIMEOUT (1000)


#define MAX_SOCK_COUNT (16)

void Abort(const char* msg);

/*________________________________ main _______________________________________
*/

int main(int argc, char* argv[])
{
	WSADATA wsaData;

	SOCKET mySock;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		Abort("Nao foi possivel iniciar o WinSock.");
	}

	if ((mySock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		Abort("Nao foi possivel criar a socket de servidor.");
	}

	{
		struct sockaddr_in boundAddr = { 0 };
		int len = sizeof(boundAddr);

		boundAddr.sin_family = AF_INET;
		boundAddr.sin_port = htons(SERV_TCP_PORT);
		boundAddr.sin_addr.S_un.S_addr = inet_addr(SERV_HOST_ADDR);

		if (bind(mySock, (const struct sockaddr*)&boundAddr, len) == SOCKET_ERROR) {
			Abort("Nao foi possivel vincular a socket a rede local.");
		}
	}


	if (listen(mySock, 5) == SOCKET_ERROR) {
		Abort("Impossivel colocar a socket a escuta.");
	}


	
	int clientCount = 0;
	SOCKET clients[MAX_SOCK_COUNT] = { 0 };


	while (1) {
		fd_set set;
		FD_ZERO(&set);

		FD_SET(mySock, &set);

		for (int i = 0; i < clientCount; i++) {
			FD_SET(clients[i], &set);
		}

		int ret = select(clientCount + 1, &set, NULL, NULL, NULL);

		switch (ret) {
		case SOCKET_ERROR:
			Abort("Erro ao realizar SELECT.");
			break;
		case 0:
			break;
		default:
			if (FD_ISSET(mySock, &set)) {
				SOCKET newSock = accept(mySock, NULL, NULL);

				if (clientCount < MAX_SOCK_COUNT) {
					const char* MSG = "Numero maximo de clientes excedidos.";
					send(newSock, MSG, strlen(MSG) + 1, 0);
					closesocket(newSock);
				}
				else {
					clients[clientCount] = newSock;
					++clientCount;
				}
			}

			for (int i = 0; i < MAX_SOCK_COUNT; i++) {
				if (FD_ISSET(clients[i], &set)) {

				}
			}
			break;
		}
	}
	closesocket(mySock);

	return 0;
}

/*________________________________ Abort________________________________________
  Mostra uma mensagem de erro e o código associado ao ultimo erro com Winsocks.
  Termina a aplicacao com "exit status" a 1 (constante EXIT_FAILURE)
________________________________________________________________________________*/

void Abort(const char* msg)
{

	fprintf(stderr, "<CLI1>Erro fatal: <%s> (%d)\n", msg, WSAGetLastError());
	exit(EXIT_FAILURE);

}