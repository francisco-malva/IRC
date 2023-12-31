/*=========================== Cliente basico UDP ===============================
Este cliente destina-se a enviar mensagens passadas na linha de comando, sob
a forma de um argumento, para um servidor especifico cuja locacao e' dada
pelas seguintes constantes: SERV_HOST_ADDR (endereco IP) e SERV_UDP_PORT (porto)

O protocolo usado e' o UDP.
==============================================================================*/

#include <winsock.h>
#include <stdio.h>

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
	struct sockaddr_in serv_addr;
	char buffer[BUFFERSIZE];
	WSADATA wsaData;

	/*========================= TESTA A SINTAXE =========================*/

	if (argc != 2) {
		fprintf(stderr, "Sintaxe: %s frase_a_enviar\n", argv[0]);
		getchar(); //system("pause");
		exit(EXIT_FAILURE);
	}

	/*=============== INICIA OS WINSOCKS ==============*/

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		getchar(); //system("pause");
		exit(1);
	}

	/*=============== CRIA SOCKET PARA ENVIO/RECEPCAO DE DATAGRAMAS ==============*/

	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if (sockfd == INVALID_SOCKET)
		Abort("Impossibilidade de criar socket");

	/*================= PREENCHE ENDERECO DO SERVIDOR ====================*/

	memset((char*)&serv_addr, 0, sizeof(serv_addr)); /*Coloca a zero todos os bytes*/
	serv_addr.sin_family = AF_INET; /*Address Family: Internet*/
	serv_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR); /*IP no formato "dotted decimal" => 32 bits*/
	serv_addr.sin_port = htons(SERV_UDP_PORT); /*Host TO Netowork Short*/

	/*====================== ENVIA MENSAGEM AO SERVIDOR ==================*/

	msg_len = strlen(argv[1]);


	if (sendto(sockfd, argv[1], msg_len + 1, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
		Abort("O subsistema de comunicacao nao conseguiu aceitar o datagrama");



	struct sockaddr_in addr = { 0 };
	int len = sizeof(addr);

	getsockname(sockfd, &addr, &len);

	printf("<CLI1> Mensagem enviada pelo porto %hu... a entrega nao e' confirmada.\n", ntohs(addr.sin_port));


	if (recvfrom(sockfd, buffer, BUFFERSIZE, 0, NULL, NULL) == SOCKET_ERROR)
	{
		Abort("Nao foi possivel receber a mensagem de volta.");
	}
	else {
		printf("<CLI1> Mensagem recebida:\n\tMensagem Enviada = \"%s\"\n\tMensagem Recebida = \"%s\"\n", argv[1], buffer);
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
