#include "gpio_utils.h"

int init_bcm(){
    if (!bcm2835_init()){
        return 1;
    }

    bcm2835_gpio_fsel(LAMP1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LAMP2, BCM2835_GPIO_FSEL_OUTP);

    bcm2835_gpio_fsel(SENSOR1, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SENSOR2, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SENSOR3, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SENSOR4, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SENSOR5, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(SENSOR6, BCM2835_GPIO_FSEL_INPT);

    return 0;
}

void get_inpt_device(int sensor[]){
    sensor[0] = bcm2835_gpio_lev(SENSOR1);
    sensor[1] = bcm2835_gpio_lev(SENSOR2);
    sensor[2] = bcm2835_gpio_lev(SENSOR3);
    sensor[3] = bcm2835_gpio_lev(SENSOR4);
    sensor[4] = bcm2835_gpio_lev(SENSOR5);
    sensor[5] = bcm2835_gpio_lev(SENSOR6);
}

void get_outp_device(int lamp[]){
    lamp[0] = bcm2835_gpio_lev(LAMP1);
    lamp[1] = bcm2835_gpio_lev(LAMP2);
}


void set_outp_device(int device, int value){
    if (device == 1){
        bcm2835_gpio_write(LAMP1, value);
    }else if (device == 2){
        bcm2835_gpio_write(LAMP2, value);
    }
}