#ifndef _NCURSES_UTILS_H_
#define _NCURSES_UTILS_H_

#include <ncurses.h>

int init_screens();
void finish_screens();
WINDOW *create_newwin(int height, int width, int starty, int startx, int bbox);
void print_menu();
void print_menu_new_device();
void print_error(char error[]);
int get_device_id();
void print_waiting_for_client();
// void update_values(double temperature, double humidity, int presence[], int openning[], int air[], int lamp[]);
void clear_input();
void print_alarm_status(char msg[]);
float get_temperature();
void print_air_status(char msg[]);
void get_comodo_name(char * buff);

void update_values(int count, char rooms[][100], int *room_temperature, int *room_humidity, int *room_led,
            int *room_button, int *openning, int *lamp);

#endif /* _NCURSES_UTILS_H_ */