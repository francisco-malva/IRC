/*=========================== Cliente basico UDP ===============================
Este cliente destina-se a enviar mensagens passadas na linha de comando, sob
a forma de um argumento, para um servidor especifico cuja locacao e' dada
pelas seguintes constantes: SERV_HOST_ADDR (endereco IP) e SERV_UDP_PORT (porto)

O protocolo usado e' o UDP.
==============================================================================*/

#include <winsock.h>
#include <stdio.h>
#include <stdint.h>

#include "../Common/args.h"

#pragma comment (lib, "Ws2_32.lib")

#define SERV_HOST_ADDR "127.0.0.1"
#define SERV_UDP_PORT  6000

#define BUFFERSIZE     4096


void Abort(char* msg);

/*________________________________ main _______________________________________
*/

int main(int argc, char* argv[])
{
	SOCKET sockfd;
	int msg_len, iResult;
	struct sockaddr_in servAddr;
	char buffer[BUFFERSIZE];
	WSADATA wsaData;

	/*=============== INICIA OS WINSOCKS ==============*/

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		getchar(); //system("pause");
		exit(1);
	}

	ArgsInit(argc, argv);

	const char* msg = ArgsGet("msg"), * ip = ArgsGet("ip"), * port = ArgsGet("port");


	if (!msg || !ip || !port) {
		puts("Utilização:\n-msg [MENSAGEM A ENVIAR]\n-ip [ENDEREÇO IP DO SERVIDOR]\n-port [PORTO DO SERVIDOR]\n");
		return -1;
	}

	char* endPtr;

	long nPort = strtol(port, &endPtr, 0);

	if (endPtr == port || nPort < 0 || nPort > UINT16_MAX) {
		Abort("Porto com formato inválido");
	}

	/*=============== CRIA SOCKET PARA ENVIO/RECEPCAO DE DATAGRAMAS ==============*/

	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if (sockfd == INVALID_SOCKET)
		Abort("Impossibilidade de criar socket");

	/*================= PREENCHE ENDERECO DO SERVIDOR ====================*/

	memset((char*)&servAddr, 0, sizeof(servAddr)); /*Coloca a zero todos os bytes*/
	servAddr.sin_family = AF_INET; /*Address Family: Internet*/
	servAddr.sin_addr.s_addr = inet_addr(ip); /*IP no formato "dotted decimal" => 32 bits*/
	servAddr.sin_port = htons((u_short)nPort); /*Host TO Netowork Short*/

	/*====================== ENVIA MENSAGEM AO SERVIDOR ==================*/

	msg_len = strlen(msg);


	if (sendto(sockfd, msg, msg_len + 1, 0, (struct sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		Abort("O subsistema de comunicacao nao conseguiu aceitar o datagrama");


	{
		struct sockaddr_in myAddr = { 0 };
		int myLen = sizeof(myAddr);

		getsockname(sockfd, &myAddr, &myLen);

		printf("<CLI1> Mensagem enviada pelo porto %hu... a entrega nao e' confirmada.\n", ntohs(myAddr.sin_port));
	}

	{
		struct sockaddr_in fromAddr = { 0 };
		int fromLen = sizeof(fromAddr);

		if (recvfrom(sockfd, buffer, BUFFERSIZE, 0, &fromAddr, &fromLen) == SOCKET_ERROR)
		{
			Abort("Nao foi possivel receber a mensagem de volta.");
		}
		else {

			int isFromServer = memcmp(&fromAddr.sin_addr, &servAddr.sin_addr, sizeof(fromAddr.sin_addr)) == 0 && ntohs(fromAddr.sin_port) == ntohs(servAddr.sin_port);

			if (isFromServer) {
				printf("<CLI1> Mensagem recebida:\n\tMensagem Enviada = \"%s\"\n\tMensagem Recebida = \"%s\"\n", msg, buffer);
			}
			else {
				printf("<CLI1> Servidor desconhecido!");
			}

		}
	}

	/*========================= FECHA O SOCKET ===========================*/

	closesocket(sockfd);
	WSACleanup();

	printf("\n");
	getchar();
	exit(EXIT_SUCCESS);
}

/*________________________________ Abort________________________________________
  Mostra uma mensagem de erro e o código associado ao ultimo erro com Winsocks.
  Termina a aplicacao com "exit status" a 1 (constante EXIT_FAILURE)
________________________________________________________________________________*/

void Abort(char* msg)
{

	fprintf(stderr, "<CLI1>Erro fatal: <%s> (%d)\n", msg, WSAGetLastError());
	exit(EXIT_FAILURE);

}