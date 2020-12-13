#ifndef MQTT_H
#define MQTT_H

void mqtt_start();

void mqtt_envia_mensagem(char * topico, char * mensagem);

void mqtt_send_button(int button_status);

char * get_comodo_mqtt_url();

#endif