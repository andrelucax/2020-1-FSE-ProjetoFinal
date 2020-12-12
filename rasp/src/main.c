#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "ncurses_utils.h"
#include "tcp_client.h"
#include "gpio_defines.h"
#include "tcp_server.h"
#include "alarm.h"
#include "log.h"

void *watch_userinput();

pthread_t thread_userinput;
// pthread_t thread_alarm;

int alarm = 0;

int main(){
    if(init_screens()){
        exit(-1);
    }

    if (pthread_create(&thread_userinput, NULL, watch_userinput, NULL)){
        exit(-2);
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

void *watch_userinput(){
    int menuOption;
    while ((menuOption = getch()) != KEY_F(1)){
    //     if (menuOption == KEY_F(2)){
    //         // Turn lamp on
    //         int lamp_id = get_device_id(LAMP);
    //         if(lamp_id < 1 || lamp_id > 4){
    //             save_in_log("Turn lamp on", "Failed (invalid ID)");
    //             print_error("Invalid ID");
    //             continue;
    //         }
    //         int server_return = send_message_to_server(LAMP, lamp_id, ON);
    //         if(server_return){
    //             save_in_log("Turn lamp on", "Failed (server)");
    //             print_error("Bad server return");
    //         }
    //         else{
    //             char log_msg[50] = "";
    //             sprintf(log_msg, "Turn lamp %d on", lamp_id);
    //             save_in_log(log_msg, "Ok");
    //         }
    //     }
    //     else if (menuOption == KEY_F(3)){
    //         // Turn lamp off
    //         int lamp_id = get_device_id(LAMP);
    //         if(lamp_id < 1 || lamp_id > 4){
    //             save_in_log("Turn lamp off", "Failed (invalid ID)");
    //             print_error("Invalid ID");
    //             continue;
    //         }
    //         int server_return = send_message_to_server(LAMP, lamp_id, OFF);
    //         if(server_return){
    //             save_in_log("Turn lamp off", "Failed (server)");
    //             print_error("Bad server return");
    //         }
    //         else{
    //             char log_msg[50] = "";
    //             sprintf(log_msg, "Turn lamp %d off", lamp_id);
    //             save_in_log(log_msg, "Ok");
    //         }
    //     }
    //     else if (menuOption == KEY_F(4)){
    //         // Turn air on
    //         int air_id = get_device_id(AIR);
    //         if(air_id < 1 || air_id > 2){
    //             save_in_log("Turn air on", "Failed (invalid ID)");
    //             print_error("Invalid ID");
    //             continue;
    //         }
    //         int server_return = send_message_to_server(AIR, air_id, ON);
    //         if(server_return){
    //             save_in_log("Turn air on", "Failed (server)");
    //             print_error("Bad server return");
    //         }
    //         else{
    //             print_air_status("# Air status: Manual                             ");
    //             char log_msg[50] = "";
    //             sprintf(log_msg, "Turn air %d on", air_id);
    //             save_in_log(log_msg, "Ok");
    //         }
    //     }
    //     else if (menuOption == KEY_F(5)){
    //         // Turn air off
    //         int air_id = get_device_id(AIR);
    //         if(air_id < 1 || air_id > 2){
    //             save_in_log("Turn air off", "Failed (invalid ID)");
    //             print_error("Invalid ID");
    //             continue;
    //         }
    //         int server_return = send_message_to_server(AIR, air_id, OFF);
    //         if(server_return){
    //             save_in_log("Turn air off", "Failed (server)");
    //             print_error("Bad server return");
    //         }
    //         else{
    //             print_air_status("# Air status: Manual                             ");
    //             char log_msg[50] = "";
    //             sprintf(log_msg, "Turn air %d off", air_id);
    //             save_in_log(log_msg, "Ok");
    //         }
    //     }
    //     else if(menuOption == KEY_F(6)){
    //         // Define air temperature
    //         float air_temperature = get_temperature();
    //         if(air_temperature < 0.0f || air_temperature > 100.0f){
    //             save_in_log("Define temperature", "Failed (invalid temperature)");
    //             print_error("Invalid temperature");
    //             continue;
    //         }
    //         int server_return = send_message_to_server_f(TEMPERATURE, air_temperature);
    //         if(server_return){
    //             save_in_log("Define temperature", "Failed (server)");
    //             print_error("Bad server return");
    //         }
    //         else{
    //             char msg_air_status[50] = "";
    //             sprintf(msg_air_status, "# Air status: Auto, reference: %.2f     ", air_temperature);
    //             print_air_status(msg_air_status);
    //             char log_msg[50] = "";
    //             sprintf(log_msg, "Define temperature %.2f", air_temperature);
    //             save_in_log(log_msg, "Ok");
    //         }
    //     }
    //     else if (menuOption == KEY_F(7)){
    //         alarm = 1;
    //         clear_input();
    //         print_alarm_status("# Alarm status: Activated              ");
    //         save_in_log("Turn alarm on", "Ok");
    //     }
    //     else if (menuOption == KEY_F(8)){
    //         alarm = 0;
    //         clear_input();
    //         print_alarm_status("# Alarm status: Deactivated            ");
    //         save_in_log("Turn alarm off", "Ok");
    //     }
    }
    // save_in_log("Exit app", "Ok");

    return 0;
}