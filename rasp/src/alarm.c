#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#include "alarm.h"
#include "log.h"
#include "gpio_defines.h"

int g_presence[2], g_openning[6], g_is_playing_sound = 0;
sem_t sem_alarm;

void check_alarm(int presence[], int openning[]){
    for(int i = 0; i < 2; i++){
        if(g_presence[i] != presence[i]){
            g_presence[i] = presence[i];
            if(g_presence[i] == ON){
                // Log alarm
                char log_msg[50] = "";
                sprintf(log_msg, "Detected presence in sensor %d", i+1);
                save_alarm_in_log(log_msg);
            }
        }
        if(!g_is_playing_sound && g_presence[i] == ON){
            g_is_playing_sound = 1;
            // Play sound
            sem_post(&sem_alarm);
        }
    }
    for(int i = 0; i < 6; i++){
        if(g_openning[i] != openning[i]){
            g_openning[i] = openning[i];
            if(g_openning[i] == ON){
                // Log alarm
                char log_msg[50] = "";
                sprintf(log_msg, "Detected openning in sensor %d", i+1);
                save_alarm_in_log(log_msg);
            }
        }
        if(!g_is_playing_sound && g_openning[i] == ON){
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
