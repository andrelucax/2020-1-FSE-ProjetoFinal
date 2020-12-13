#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tcp_server.h"
#include "ncurses_utils.h"
#include "alarm.h"

int client_socket;
int server_socket;
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;

int init_tcp_server(){
	unsigned short server_port;

	server_port = MY_SERVER_PORT;

	// Abrir Socket
	if((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        return 1;
    }

	// Montar a estrutura sockaddr_in
	memset(&server_addr, 0, sizeof(server_addr)); // Zerando a estrutura de dados
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(server_port);

	// Bind
	if(bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        return 2;
    }

	// Listen
	if(listen(server_socket, 10) < 0){
        return 3;
    }

    return 0;
}

int wait_for_client(){
	unsigned int client_len = sizeof(client_addr);
	if((client_socket = accept(server_socket, 
								(struct sockaddr *) &client_addr, 
								&client_len)) < 0){
		printf("Error on accept\n");
		return 1;
	}
	return 0;
}

void *handle_tcp_client(int *alarm) {
	while(1){
		if(wait_for_client()){
			continue;
		}
		// int is_ok = 1;
		int recv_size;
		double humidity, temperature;
		int presence[2], openning[6], air[2], lamp[4];
		if((recv_size = recv(client_socket, (void *) &humidity, sizeof(double), 0)) < 0 ||
		   (recv_size = recv(client_socket, (void *) &temperature, sizeof(double), 0)) < 0){
			// is_ok = 0;
		}
		if((recv_size = recv(client_socket, (void *) presence, sizeof(int)*2, 0)) < 0){
			// is_ok = 0;
		}
		if((recv_size = recv(client_socket, (void *) openning, sizeof(int)*6, 0)) < 0){
			// is_ok = 0;
		}
		if((recv_size = recv(client_socket, (void *) air, sizeof(int)*2, 0)) < 0){
			// is_ok = 0;
		}
		if((recv_size = recv(client_socket, (void *) lamp, sizeof(int)*4, 0)) < 0){
			// is_ok = 0;
		}

		// update_values(temperature, humidity, presence, openning, air, lamp);
		if(*alarm){
			check_alarm(presence, openning);
		}

		close(client_socket);
	}
}

void close_sockets(){
    close(server_socket);
    close(client_socket);
}
