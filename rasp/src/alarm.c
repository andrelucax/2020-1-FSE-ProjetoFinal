#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#include "alarm.h"
#include "log.h"
#include "gpio_utils.h"

int g_sensors[6], g_is_playing_sound = 0;
int g_devices[5];
sem_t sem_alarm;

void check_alarm(int sensors[], int devices[], int count){
    for(int i=0; i<6; i++){
        if(g_sensors[i] != sensors[i]){
            g_sensors[i] = sensors[i];
            if(g_sensors[i] == ON){
                // Log alarm
                char log_msg[50] = "";
                sprintf(log_msg, "Detected presence in sensor %d", i+1);
                save_alarm_in_log(log_msg);
            }
        }
        if(!g_is_playing_sound && sensors[i] == ON){
            g_is_playing_sound = 1;
            // Play sound
            sem_post(&sem_alarm);
        }
    }

    for(int i=0; i<count; i++){
        if(g_devices[i] != devices[i]){
            g_devices[i] = devices[i];
            if(g_devices[i] == ON){
                // Log alarm
                char log_msg[50] = "";
                sprintf(log_msg, "Detected presence in sensor of device %d", i+3);
                save_alarm_in_log(log_msg);
            }
        }
        if(!g_is_playing_sound && devices[i] == ON){
            g_is_playing_sound = 1;
            // Play sound
            sem_post(&sem_alarm);
        }
    }
}

void *handle_alarm(){
    sem_init(&sem_alarm, 0, 0);
    while(1){
        sem_wait(&sem_alarm);
        system("omxplayer yamete.mp3 > /dev/null 2>&1");
        // When stop sound
        g_is_playing_sound = 0;
    }
}
