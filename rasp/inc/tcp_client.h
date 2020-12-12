#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#define SERVER_IP "192.168.0.52"
#define OTHER_SERVER_PORT 10129

int init_tcp_client();
int send_message_to_server(int device_type, int device_id, int on_off);
void close_socket();
int send_message_to_server_f(int command, float air_temperature);

#endif // _TCP_CLIENT_H_