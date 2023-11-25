#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "lib/log.h"
#include "lib/fhandler.h"
#include "lib/Misc.h"

#define _MAX_STRIKE_ 7


int string_included(user_history_t history, const char *path){
    for (int j = 0; j < history.strikes; j++){
        if (strcmp(history.path[j], path) == 0){
            return 1;
        }
    }
    return 0;
}

void print_logf(logf_t log){
    char *access;
    switch (log.access){
        case __CREATION:
            access = "CREATE";
            break;
        case __OPEN:
            access = "OPEN";
            break;
        case __WRITE:
            access = "WRITE";
            break;
        case __READ:
            access = "READ";
            break;
        default:
            printf("Invalid access type\n");
            exit(-1);
    }

    printf("[%02d:%02d:%02d] %02d/%02d/%04d | UID : %5d | Action : %6s | Denied : %1d | Fingerprint : %s | Path : %s\n", log.timestamp.tm_hour, log.timestamp.tm_min, log.timestamp.tm_sec, log.timestamp.tm_mday, log.timestamp.tm_mon, log.timestamp.tm_year, log.UID, access, log.action_denied, log.fingerprint, log.path);
}

int printMaliciousUsers(){
    array_t *data = user_history_init();
    user_history_t *user_history = (user_history_t *)data->data;

    for (int i = 0; i < data->size; i++){
        if (user_history[i].strikes >= _MAX_STRIKE_)
        {
            printf("User %d has been banned\n", user_history[i].UID);
        }
    }
    return 0;
}

void printHelp(){
    return;
}

int main(int argc, char *argv[]){
    if (argc == 2){
        if(strcmp(argv[1],"-m") == 0){
            return printMaliciousUsers();
        } else if (strcmp(argv[1],"-h") == 0){
            printHelp();
        }else if (strcmp(argv[1],"-i") == 0){
            perror("No file specified");
            return -1;
        }
    } else if (argc == 3){

        array_t *data = file_history_init();
        file_history_t *history = (file_history_t *)data->data;

        file_history_t file;
        for(int i = 0; i < data->size; i++){
            if (strcmp(history[i].path, argv[2]) == 0){
                file = history[i];
                if (file.users == 0){
                    printf("File has not been modified\n");
                    exit(-1);
                }
                break;
            }
        }
        
        if (file.users == 0){
            printf("File was not found\n");
            exit(-1);
        }

        for (int i = 0; i < file.users; i++){
            printf("User %d has modified the file %d times\n", file.UID[i], file.modifications[i]);
        }

        return 1;
    } else {
        printf("Invalid parameters\n");
        return -1;
    }
}