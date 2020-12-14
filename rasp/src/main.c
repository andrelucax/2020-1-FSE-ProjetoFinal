#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#include "cJSON.h"

#include "ncurses_utils.h"
#include "gpio_utils.h"
#include "alarm.h"
#include "log.h"

#include <MQTTClient.h>

void *watch_userinput();
void *watch_updateinterface();

#define MQTT_ADDRESS "tcp://mqtt.eclipseprojects.io"
#define CLIENTID "rasp1"

char mqtt_base_topic[255] = "fse2020/170068251/#";
char mqtt_dispositivos[255] = "fse2020/170068251/dispositivos/";
MQTTClient client;

char meus_dispositivos[5][255];
char meus_comodos[5][100]; // TODO init with ""

int room_temperature[5];
int room_humidity[5];
int room_led[5];
int room_button[5];

int sensors[6];
int lamp[2];

int count_dispositivos=0;

int dispositivos_para_registrar=0;

pthread_t thread_userinput;
pthread_t thread_updateinterface;

sem_t hold_screens;
// pthread_t thread_alarm;

int alarm_status = 0;

void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);

void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 2;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    // char* payload = message->payload;

    // char __temp[256];
    // sprintf(__temp, "Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload)
    // printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload);
    char temp[1024];
    char delim[2] = "/";
    strcpy(temp, topicName);
    char *ptr = strtok(temp, delim); // fse
    ptr = strtok(NULL, delim); // matricula
    ptr = strtok(NULL, delim); // dispositivo || comodo

    if(ptr == NULL){
        //coco
    }else if(!strcmp(ptr, "dispositivos")){
        // cadastrando
        ptr = strtok(NULL, delim); // mac
        if(dispositivos_para_registrar == 5) return 1;
        // for dispositivos, ver se ja tem um ocm mesmo nome
        for(int i=0; i<count_dispositivos; ++i){
            if(!strcmp(ptr, meus_dispositivos[i])){
                // return 1;
            }
        }
        cJSON * json = cJSON_Parse(message->payload);
        int _init = 0;
        if(cJSON_IsNumber(cJSON_GetObjectItemCaseSensitive(json, "init"))){
            _init = cJSON_GetObjectItemCaseSensitive(json, "init")->valueint;
            if(_init == 1){
                strcpy(meus_dispositivos[count_dispositivos], ptr);
                dispositivos_para_registrar++;
            }
        }
        cJSON_Delete(json);
    }else{
        // já deve estar cadastrado
        // save_in_log("DEBUG - payload", message->payload); 
        for(int i=0; i<count_dispositivos; ++i){
            if(!strcmp(ptr, meus_comodos[i])){
                cJSON * json = cJSON_Parse(message->payload);
                ptr = strtok(NULL, delim); // temperatura, umididade or estado
                if (strcmp(ptr, "temperatura") == 0){
                    int _temp = cJSON_GetObjectItemCaseSensitive(json, "temperatura")->valueint;
                    room_temperature[i] = _temp;
                }else if (strcmp(ptr, "umidade") == 0){
                    int _umid = cJSON_GetObjectItemCaseSensitive(json, "umidade")->valueint;
                    room_humidity[i] = _umid;
                }else if (strcmp(ptr, "estado") == 0){
                    if(cJSON_IsNumber(cJSON_GetObjectItemCaseSensitive(json, "botao"))){
                        room_button[i] = cJSON_GetObjectItemCaseSensitive(json, "botao")->valueint;
                    }else if(cJSON_IsNumber(cJSON_GetObjectItemCaseSensitive(json, "led"))){
                        room_led[i] = cJSON_GetObjectItemCaseSensitive(json, "led")->valueint;
                    }
                }

                cJSON_Delete(json);
                break;
            }
        }
    }


    /* Faz echo da mensagem recebida */
    // publish(client, MQTT_PUBLISH_TOPIC, payload);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void init_mqtt(){
    int rc;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    /* Inicializacao do MQTT (conexao & subscribe) */
    MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    rc = MQTTClient_connect(client, &conn_opts);

    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
        exit(-1);
    }
    MQTTClient_subscribe(client, mqtt_base_topic, 0);

    for(int i=0; i<5;++i){
        strcpy(meus_comodos[i], "");
        strcpy(meus_dispositivos[i], "");
    }
}

int main(){
    // Init mqtt
    init_mqtt();

    // Init semaphore
    sem_init(&hold_screens, 0, 1);

    // Init gpio
    init_bcm();

    if(init_screens()){
        exit(-1);
    }

    if (pthread_create(&thread_userinput, NULL, watch_userinput, NULL)){
        exit(-2);
    }

    if (pthread_create(&thread_updateinterface, NULL, watch_updateinterface, NULL)){
        exit(-3);
    }

    // if (pthread_create(&thread_alarm, NULL, handle_alarm, NULL))
    // {
    //     exit(-4);
    // }

    pthread_join(thread_userinput, NULL);

    // pthread_cancel(thread_alarm);

    finish_screens();

    return 0;
}

void *watch_updateinterface(){
    while(1){
        sem_wait(&hold_screens);
        get_outp_device(lamp);
        get_inpt_device(sensors);
        print_menu_new_device(dispositivos_para_registrar);
        update_values(count_dispositivos, meus_comodos, room_temperature, room_humidity, room_led,
            room_button, sensors, lamp);
        print_alarm_status("# Alarm status: Deactivated            ");
        sem_post(&hold_screens);
        // check_alarm();
        sleep(2);
    }
}

void *watch_userinput(){
    int menuOption;
    while ((menuOption = getch()) != KEY_F(1)){
        if (menuOption == KEY_F(2)){
            // Turn lamp on
            sem_wait(&hold_screens);
            int device_id = get_device_id();
            sem_post(&hold_screens);
            if(device_id == 1 || device_id == 2){
                set_outp_device(device_id, 1);
            }else if(device_id >=3 && device_id <= 7){
                int t_comodo = device_id-3;
                if(strcmp(meus_comodos[t_comodo], "") == 0){
                    save_in_log("Turn device on", "Failed (invalid ID)");
                    print_error("Invalid ID");
                    continue;
                }else{
                    // send to mqtt 
                    char t_data[512];
                    char t_topic[512];
                    sprintf(t_topic, "%s%s", mqtt_dispositivos, meus_dispositivos[t_comodo]);
                    sprintf(t_data, "{ \"tipo\": \"comando\", \"comando\": %d}", 1);
                    publish(client, t_topic, t_data);
                }
            }else{
                save_in_log("Turn device on", "Failed (invalid ID)");
                print_error("Invalid ID");
                continue;
            }
            char log_msg[50] = "";
            sprintf(log_msg, "Turn device %d on", device_id);
            save_in_log(log_msg, "Ok");  
        }
        else if (menuOption == KEY_F(3)){
            // Turn lamp on
            sem_wait(&hold_screens);
            int device_id = get_device_id();
            sem_post(&hold_screens);
            if(device_id == 1 || device_id == 2){
                set_outp_device(device_id, 0);
            }else if(device_id >=3 && device_id <= 7){
                int t_comodo = device_id-3;
                if(strcmp(meus_comodos[t_comodo], "") == 0){
                    save_in_log("Turn device off", "Failed (invalid ID)");
                    print_error("Invalid ID");
                    continue;
                }else{
                    // send to mqtt 
                    char t_data[512];
                    char t_topic[512];
                    sprintf(t_topic, "%s%s", mqtt_dispositivos, meus_dispositivos[t_comodo]);
                    sprintf(t_data, "{ \"tipo\": \"comando\", \"comando\": %d}", 0);
                    publish(client, t_topic, t_data);
                }
            }else{
                save_in_log("Turn device off", "Failed (invalid ID)");
                print_error("Invalid ID");
                continue;
            }
            char log_msg[50] = "";
            sprintf(log_msg, "Turn device %d off", device_id);
            save_in_log(log_msg, "Ok");  
        }
        else if(menuOption == KEY_F(6)){
            if(!dispositivos_para_registrar) continue;
            char new_comodo[100];
            sem_wait(&hold_screens);
            get_comodo_name(new_comodo);
            sem_post(&hold_screens);
            int is_valid_input=1;
            if(strlen(new_comodo) < 3){
                save_in_log("Register new device", "Failed (Invalid room name: too short)");
                print_error("Invalid room name: too short");
                is_valid_input=0;
            }
            for(int i=0; i<count_dispositivos; i++){
                if(strcmp(new_comodo, meus_comodos[i]) == 0){
                    save_in_log("Register new device", "Failed (Invalid room name: already registered)");
                    print_error("Invalid room name: already registered");
                    is_valid_input=0;
                    break;
                }
            }
            if(is_valid_input){
                strcpy(meus_comodos[count_dispositivos], new_comodo);
                // send to mqtt 
                char t_data[512];
                char t_topic[512];
                sprintf(t_topic, "%s%s", mqtt_dispositivos, meus_dispositivos[count_dispositivos]);
                sprintf(t_data, "{ \"tipo\": \"define_comodo\", \"comodo\": \"%s\"}", new_comodo);
                publish(client, t_topic, t_data);
                count_dispositivos++;
                dispositivos_para_registrar--;
            }
        }
    //     else if (menuOption == KEY_F(7)){
    //         alarm_status = 1;
    //         clear_input();
    //         print_alarm_status("# Alarm status: Activated              ");
    //         save_in_log("Turn alarm on", "Ok");
    //     }
    //     else if (menuOption == KEY_F(8)){
    //         alarm_status = 0;
    //         clear_input();
    //         print_alarm_status("# Alarm status: Deactivated            ");
    //         save_in_log("Turn alarm off", "Ok");
    //     }
    }
    save_in_log("Exit app", "Ok");

    return 0;
}