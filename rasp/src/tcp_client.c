#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tcp_client.h"

int client_socket;
struct sockaddr_in server_addr;

int init_tcp_client() {
	unsigned short server_port;
	char *server_IP;

	server_IP = SERVER_IP;
	server_port = OTHER_SERVER_PORT;

	// Criar Socket
	if((client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		return 1;

	// Construir struct sockaddr_in
	memset(&server_addr, 0, sizeof(server_addr)); // Zerando a estrutura de dados
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_IP);
	server_addr.sin_port = htons(server_port);

	// tamanhoMensagem = strlen(mensagem);

	// if(send(client_socket, mensagem, tamanhoMensagem, 0) != tamanhoMensagem)
	// 	printf("Erro no envio: numero de bytes enviados diferente do esperado\n");

	// totalBytesRecebidos = 0;
	// while(totalBytesRecebidos < tamanhoMensagem) {
	// 	if((bytesRecebidos = recv(client_socket, buffer, 16-1, 0)) <= 0)
	// 		printf("Não recebeu o total de bytes enviados\n");
	// 	totalBytesRecebidos += bytesRecebidos;
	// 	buffer[bytesRecebidos] = '\0';
	// 	printf("%s\n", buffer);
	// }
	// close(client_socket);
	// exit(0);

	return 0;
}

int send_message_to_server_f(int command, float air_temperature){
	if(init_tcp_client()){
        return 1;
    }
	// Connect
	if(connect(client_socket, (struct sockaddr *) &server_addr, 
							sizeof(server_addr)) < 0)
		return 2;

	int is_ok = 0;
	if(send(client_socket, (void *) &command, sizeof(int), 0) < 0)
		is_ok = 3;

    if(send(client_socket, (void *) &air_temperature, sizeof(float), 0) < 0)
		is_ok = 3;

	int server_ans = 0;
    if(recv(client_socket, (void *) &server_ans, sizeof(int), 0) < 0){
        is_ok = 4;
    }

	close_socket();

    if(is_ok){
        return is_ok;
    }

	if(server_ans){
		return 5;
	}

    return 0;
}

int send_message_to_server(int device_type, int device_id, int on_off){
	if(init_tcp_client()){
        return 1;
    }
	// Connect
	if(connect(client_socket, (struct sockaddr *) &server_addr, 
							sizeof(server_addr)) < 0)
		return 2;

    int is_ok = 0;
    if(send(client_socket, (void *) &device_type, sizeof(int), 0) < 0)
		is_ok = 3;

    if(send(client_socket, (void *) &device_id, sizeof(int), 0) < 0)
		is_ok = 3;

    if(send(client_socket, (void *) &on_off, sizeof(int), 0) < 0)
		is_ok = 3;

	int server_ans = 0;
    if(recv(client_socket, (void *) &server_ans, sizeof(int), 0) < 0){
        is_ok = 4;
    }

	close_socket();

    if(is_ok){
        return is_ok;
    }

	if(server_ans){
		return 5;
	}

    return 0;
}

void close_socket(){
	close(client_socket);
}