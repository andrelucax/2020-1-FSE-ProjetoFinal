#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"

#include "mqtt.h"
#include "nvs_utils.h"

#define TAG "MQTT"
#define LED 2

char mqtt_url[100] = "fse2020/170068251/dispositivos/";

extern xSemaphoreHandle conexaoMQTTSemaphore1;
extern xSemaphoreHandle conexaoMQTTSemaphore2;
char comodo_mqtt_url[200];
esp_mqtt_client_handle_t client;

char * get_comodo_mqtt_url(){
    return comodo_mqtt_url;
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            char valor_lido[100] = "";
            int res = le_valor_nvs(valor_lido);
            // strcpy(valor_lido, le_valor_nvs());
            if(!res){
                sprintf (comodo_mqtt_url, "fse2020/<matricula>/%s/", valor_lido);
                xSemaphoreGive(conexaoMQTTSemaphore1);
                xSemaphoreGive(conexaoMQTTSemaphore2);
            }
            else{
                mqtt_envia_mensagem(mqtt_url, "{ init: 1 }");
            }
            msg_id = esp_mqtt_client_subscribe(client, mqtt_url, 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            cJSON * json = cJSON_Parse (event->data);
            char opt[100];
            strcpy(opt, cJSON_GetObjectItemCaseSensitive(json, "tipo")->valuestring);
            if(strcmp(opt, "comando") == 0){
                int comando = cJSON_GetObjectItemCaseSensitive(json, "comando")->valueint;
                char tmp[300];
                sprintf(tmp, "%s%s", comodo_mqtt_url, "estado");
                if (comando){
                    gpio_set_level(LED, 1);
                    mqtt_envia_mensagem(tmp, "{ led: 1 }");
                }
                else{
                    gpio_set_level(LED, 0);
                    mqtt_envia_mensagem(tmp, "{ led: 0 }");
                }
            }
            else if(strcmp(opt, "define_comodo") == 0){
                char comodo[100];
                strcpy(comodo, cJSON_GetObjectItemCaseSensitive(json, "comodo")->valuestring);
                sprintf (comodo_mqtt_url, "fse2020/<matricula>/%s/", comodo);
                grava_valor_nvs(comodo);
                xSemaphoreGive(conexaoMQTTSemaphore1);
                xSemaphoreGive(conexaoMQTTSemaphore2);
            }
            cJSON_Delete(json);
            // printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

void mqtt_send_button(){
    char tmp[300];
    sprintf(tmp, "%s%s", comodo_mqtt_url, "estado");
    mqtt_envia_mensagem(tmp, "{ botao: 1 }");
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_start()
{
    uint8_t mac;
    esp_efuse_mac_get_default(&mac);
    char buffer[80];
    strcpy(buffer, mqtt_url);
    sprintf(mqtt_url, "%s%d", buffer, mac);
    esp_mqtt_client_config_t mqtt_config = {
        .uri = "mqtt://mqtt.eclipseprojects.io",
    };
    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void mqtt_envia_mensagem(char * topico, char * mensagem)
{
    int message_id = esp_mqtt_client_publish(client, topico, mensagem, 0, 1, 0);
    ESP_LOGI(TAG, "Mesnagem enviada, ID: %d", message_id);
}