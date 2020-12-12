#ifndef _NCURSES_UTILS_H_
#define _NCURSES_UTILS_H_

#include <ncurses.h>

int init_screens();
void finish_screens();
WINDOW *create_newwin(int height, int width, int starty, int startx, int bbox);
void print_menu();
void print_error(char error[]);
int get_device_id(int device_type);
void print_waiting_for_client();
void update_values(double temperature, double humidity, int presence[], int openning[], int air[], int lamp[]);
void clear_input();
void print_alarm_status(char msg[]);
float get_temperature();
void print_air_status(char msg[]);

#endif /* _NCURSES_UTILS_H_ */