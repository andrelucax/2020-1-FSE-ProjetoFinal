#include <time.h>
#include <stdio.h>

#include "log.h"

void save_in_log(char msg[], char status[]){
    // Open CSV file
    FILE *arq;
    arq = fopen("./commands.csv", "r+");
    if (arq)
    {
        // If exists go to end
        fseek(arq, 0, SEEK_END);
    }
    else
    {
        // Oppen in append mode
        arq = fopen("./commands.csv", "a");

        // header
        fprintf(arq, "Command, Status, Data\n");
    }

    if (arq)
    {
        time_t rawtime;
        struct tm *timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        fprintf(arq, "%s, %s, %s", msg, status, asctime(timeinfo));
    }

    // close CSV file
    fclose(arq);
}

void save_alarm_in_log(char msg[]){
        // Open CSV file
    FILE *arq;
    arq = fopen("./alarms.csv", "r+");
    if (arq)
    {
        // If exists go to end
        fseek(arq, 0, SEEK_END);
    }
    else
    {
        // Oppen in append mode
        arq = fopen("./alarms.csv", "a");

        // header
        fprintf(arq, "Alarm, Data\n");
    }

    if (arq)
    {
        time_t rawtime;
        struct tm *timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        fprintf(arq, "%s, %s", msg, asctime(timeinfo));
    }

    // close CSV file
    fclose(arq);
}