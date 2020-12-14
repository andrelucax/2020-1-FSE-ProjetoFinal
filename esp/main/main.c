#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include <string.h>
#include "driver/gpio.h"
#include "dht11.h"

#include "wifi.h"
#include "mqtt.h"
#include "nvs_utils.h"

#define LED 2
#define BUTTON 0

char * response_buffer = NULL;
int response_size = 0;

int wifi_connected = 0;

int button_status = 0;

xSemaphoreHandle conexaoWifiSemaphore;
xSemaphoreHandle conexaoMQTTSemaphore1;
xSemaphoreHandle conexaoMQTTSemaphore2;

xQueueHandle filaDeInterrupcao;

void conectadoWifi(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      // Processamento Internet
      mqtt_start();
    }
  }
}

static void IRAM_ATTR gpio_isr_handler(void *args)
{
  int pino = (int)args;
  xQueueSendFromISR(filaDeInterrupcao, &pino, NULL);
}

void botaoHandler(void * params){
  int pino;
  if(xSemaphoreTake(conexaoMQTTSemaphore2, portMAX_DELAY)){
    while(true)
    {
      if(xQueueReceive(filaDeInterrupcao, &pino, portMAX_DELAY))
      {
        // De-bouncing
        int estado = gpio_get_level(pino);
        if(estado == 1)
        {
          gpio_isr_handler_remove(pino);
          while(gpio_get_level(pino) == estado)
          {
            vTaskDelay(50 / portTICK_PERIOD_MS);
          }

          if (button_status == 0){
            button_status = 1;
          }
          else{
            button_status = 0;
          }
          mqtt_send_button(button_status);

          // Habilitar novamente a interrupção
          vTaskDelay(50 / portTICK_PERIOD_MS);
          gpio_isr_handler_add(pino, gpio_isr_handler, (void *) pino);
        }

      }
    }
  }
}

void trataComunicacaoComServidor(void * params)
{
  char mensagem_temperatura[50];
  char mensagem_umidade[50];
  // char mensagem_estado[50];
  if(xSemaphoreTake(conexaoMQTTSemaphore1, portMAX_DELAY))
  {
    struct dht11_reading data_dht11;
    while(true)
    {
      data_dht11 = DHT11_read();

      if(data_dht11.status){
        ESP_LOGE("DHTERROR", "DHT_11_FAIL_TO_READ");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        continue;
      }

      sprintf(mensagem_temperatura, "{ \"temperatura\": %d }", data_dht11.temperature);
      sprintf(mensagem_umidade, "{ \"umidade\": %d }", data_dht11.humidity);
      // sprintf(mensagem_estado, "%d", data_dht11.status);

      char temperatura_mqtt_url[300];
      char umidade_mqtt_url[300];
      // char estado_mqtt_url[300];

      char buf[200];
      strcpy(buf, get_comodo_mqtt_url());

      sprintf(temperatura_mqtt_url, "%s%s", buf, "temperatura");
      sprintf(umidade_mqtt_url, "%s%s", buf, "umidade");
      // sprintf(estado_mqtt_url, "%s%s", buf, "estado");

      mqtt_envia_mensagem(temperatura_mqtt_url, mensagem_temperatura);
      mqtt_envia_mensagem(umidade_mqtt_url, mensagem_umidade);
      // mqtt_envia_mensagem(estado_mqtt_url, mensagem_estado);

      vTaskDelay(30 * 1000 / portTICK_PERIOD_MS);
    }
  }
}

void app_main(void)
{
  DHT11_init(GPIO_NUM_4);
  // Inicializa o NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Inicializa LED
  gpio_pad_select_gpio(LED);
  gpio_set_direction(LED, GPIO_MODE_OUTPUT);

  // Inicializa BUTTON
  gpio_pad_select_gpio(BUTTON);
  gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
  gpio_pulldown_en(BUTTON);
  gpio_pullup_dis(BUTTON);
  gpio_set_intr_type(BUTTON, GPIO_INTR_POSEDGE);
  
  conexaoWifiSemaphore = xSemaphoreCreateBinary();
  conexaoMQTTSemaphore1 = xSemaphoreCreateBinary();
  conexaoMQTTSemaphore2 = xSemaphoreCreateBinary();
  wifi_start();

  filaDeInterrupcao = xQueueCreate(10, sizeof(int));
  xTaskCreate(&botaoHandler, "Pega Botao", 4096, NULL, 1, NULL);
  gpio_install_isr_service(0);
  gpio_isr_handler_add(BUTTON, gpio_isr_handler, (void *) BUTTON);

  xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
  xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
}
