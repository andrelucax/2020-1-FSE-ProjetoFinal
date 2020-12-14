#include <ncurses_utils.h>
#include <string.h>
#include <float.h>

#include "gpio_utils.h"

WINDOW *menuWindow;
WINDOW *menuBoxWindow;

WINDOW *inputWindow;
WINDOW *inputBoxWindow;

WINDOW *dataWindow;
WINDOW *dataBoxWindow;

int init_screens()
{
    initscr();            // Init curses mode
    noecho();             // Don't need to see user input
    cbreak();             // Disable line buffering, gimme every thing
    keypad(stdscr, TRUE); // Gimme that spicy F button
    curs_set(0);          // Hide cursor
    refresh();

    int row, col;
    getmaxyx(stdscr, row, col);
    if (row < 31 || col < 120)
    {
        endwin();

        printf("Screen too small, min rows = 31 and min columns = 120\n\rGot rows = %d and columns = %d\n\rPlease resize your screen\n", row, col);

        return -1;
    }

    menuWindow = create_newwin(LINES - 3, COLS / 2, 0, 0, 0);
    menuBoxWindow = create_newwin(LINES - 3, COLS / 2, 0, 0, 1);

    inputWindow = create_newwin(3, COLS / 2, LINES - 3, 0, 0);
    inputBoxWindow = create_newwin(3, COLS / 2, LINES - 3, 0, 1);

    dataWindow = create_newwin(LINES, COLS / 2, 0, COLS / 2, 0);
    dataBoxWindow = create_newwin(LINES, COLS / 2, 0, COLS / 2, 1);

    print_menu();
    // print_waiting_for_client();

    print_alarm_status("# Alarm status: Deactivated            ");
    // print_air_status("# Air status: Manual                             ");

    return 0;
}

void finish_screens()
{
    delwin(menuBoxWindow);
    delwin(menuWindow);

    delwin(inputWindow);
    delwin(inputBoxWindow);

    delwin(dataWindow);
    delwin(dataBoxWindow);

    endwin();
}

WINDOW *create_newwin(int height, int width, int starty, int startx, int bbox)
{
    if (!bbox)
    {
        height -= 2;
        width -= 2;
        starty += 1;
        startx += 1;
    }

    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);

    if (bbox)
    {
        box(local_win, 0, 0);
    }

    wrefresh(local_win);

    return local_win;
}

void print_menu()
{
    wclear(menuWindow);

    mvwprintw(menuWindow, 0 , 0, "Option F1: exit program");


    mvwprintw(menuWindow, 3 , 0, "Option F2: turn device on");
    mvwprintw(menuWindow, 4 , 0, "Option F3: turn device off");

    mvwprintw(menuWindow, 6, 0, "Option F4: activate alarm");
    mvwprintw(menuWindow, 7, 0, "Option F5: deactivate alarm");

    wrefresh(menuWindow);
}

void print_menu_new_device(int dispositivos_para_registrar){
    if(dispositivos_para_registrar){
        mvwprintw(menuWindow, 9, 0, "Option F6: register new device");
    }else{
        mvwprintw(menuWindow, 9, 0, "                                ");
    }
    wrefresh(menuWindow);
}

// void print_waiting_for_client(){
//     wclear(dataWindow);
//     mvwprintw(dataWindow, 0, 0, "Waiting for client");
//     wrefresh(dataWindow);
// }

void print_error(char error[]){
    wclear(inputWindow);

    mvwprintw(inputWindow, 0, 0, error);

    wrefresh(inputWindow);
}

float get_temperature(){
    wclear(inputWindow);

    echo();

    float temperature;
    mvwprintw(inputWindow, 0, 0, "Reference temperature > ");

    wrefresh(inputWindow);
    wscanw(inputWindow, "%f", &temperature);

    wclear(inputWindow);
    wrefresh(inputWindow);

    noecho();

    return temperature;
}

void get_comodo_name(char * buff){
    wclear(inputWindow);

    echo();
    mvwprintw(inputWindow, 0, 0, "Enter room name for new device > ");

    wrefresh(inputWindow);
    wscanw(inputWindow, "%s", buff);

    wclear(inputWindow);
    wrefresh(inputWindow);

    noecho();
}

int get_device_id(){
    wclear(inputWindow);

    echo();

    int device_id = 0;

    mvwprintw(inputWindow, 0, 0, "Enter device id > ");
  

    wrefresh(inputWindow);
    wscanw(inputWindow, "%d", &device_id);

    wclear(inputWindow);
    wrefresh(inputWindow);

    noecho();

    return device_id;
}

void update_values(int count, char rooms[][100], int *room_temperature, int *room_humidity, int *room_led,
            int *room_button, int *openning, int *lamp)
{
    if(lamp[0] == ON)
        mvwprintw(dataWindow, 0 , 0, "Kitchen lamp (1): ------------- ON            ");
    else
        mvwprintw(dataWindow, 0 , 0, "Kitchen lamp (1): ------------- OFF           ");

    if(lamp[1] == ON)
        mvwprintw(dataWindow, 1 , 0, "Living room lamp (2): --------- ON            ");
    else
        mvwprintw(dataWindow, 1 , 0, "Living room lamp (2): --------- OFF           ");

    if(openning[0] == ON)
        mvwprintw(dataWindow, 2 , 0, "Living room presence: --------- Detected           ");
    else
        mvwprintw(dataWindow, 2 , 0, "Living room presence: --------- Nothing          ");

    if(openning[1] == ON)
        mvwprintw(dataWindow, 3 , 0, "Kitchen presence: ------------- Detected           ");
    else
        mvwprintw(dataWindow, 3 , 0, "Kitchen presence: ------------- Nothing          ");

    if(openning[2] == ON)
        mvwprintw(dataWindow, 4 , 0, "Kitchen door presence: -------- Detected           ");
    else
        mvwprintw(dataWindow, 4 , 0, "Kitchen door presence: -------- Nothing          ");

    if(openning[3] == ON)
        mvwprintw(dataWindow, 5 , 0, "Kitchen window presence: ------ Detected           ");
    else
        mvwprintw(dataWindow, 5 , 0, "Kitchen window presence: ------ Nothing          ");

    if(openning[4] == ON)
        mvwprintw(dataWindow, 6 , 0, "Living room door presence: ---- Detected           ");
    else
        mvwprintw(dataWindow, 6 , 0, "Living room door presence: ---- Nothing          ");

    if(openning[5] == ON)
        mvwprintw(dataWindow, 7 , 0, "Living room window presence: -- Detected           ");
    else
        mvwprintw(dataWindow, 7 , 0, "Living room window presence: -- Nothing          ");

    int pos = 8;
    for(int i=0; i<count; ++i){
        mvwprintw(dataWindow, pos , 0, "Room name: %s (%d) {              ", rooms[i], i+3);
        pos++;
        mvwprintw(dataWindow, pos , 0, "  Temperature: %d oC, Humidity: %d %%        ", 
            room_temperature[i], room_humidity[i]);
        pos++;
        mvwprintw(dataWindow, pos , 0, "  Lamp: %s, Presence: %s    \n}",
            room_led[i] ? "ON" : "OFF",
            room_button[i] ? "Detected" : "Nothing");
        pos +=2;
    }

    wrefresh(dataWindow);
}


void clear_input(){
    wclear(inputWindow);
    wrefresh(inputWindow);
}

void print_alarm_status(char msg[]){
    mvwprintw(dataWindow, 29, 0, msg);
    wrefresh(dataWindow);
}

void print_air_status(char msg[]){
    mvwprintw(dataWindow, 23, 0, msg);
    wrefresh(dataWindow);
}