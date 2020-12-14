#ifndef _GPIO_UTILS_H_
#define _GPIO_UTILS_H_

#include <bcm2835.h>

#define ON 1
#define OFF 0

#define LAMP1 RPI_V2_GPIO_P1_11 // KITCHEN
#define LAMP2 RPI_V2_GPIO_P1_12 // LIVING ROOM

#define SENSOR1 RPI_V2_GPIO_P1_22 // LIVING ROOM
#define SENSOR2 RPI_V2_GPIO_P1_37 // KITCHEN
#define SENSOR3 RPI_V2_GPIO_P1_29 // KITCHEN DOOR
#define SENSOR4 RPI_V2_GPIO_P1_31 // KITCHEN WINDOW
#define SENSOR5 RPI_V2_GPIO_P1_32 // LIVING ROOM DOOR
#define SENSOR6 RPI_V2_GPIO_P1_36 // LIVING ROOM WINDOW

int init_bcm();
void get_inpt_device(int sensor[]);
void get_outp_device(int lamp[]);
void set_outp_device(int device, int value);

#endif // _GPIO_UTILS_H_