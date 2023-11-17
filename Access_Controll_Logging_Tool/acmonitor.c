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

int main(int argc, char *argv[]){

    if (argc == 2){
        array_t *data = file_history_init();
        file_history_t *history = (file_history_t *)data->data;

        file_history_t file;
        for(int i = 0; i < data->size; i++){
            if (strcmp(history[i].path, argv[1]) == 0){
                file = history[i];
            }
        }
        if (file.users == 0){
            printf("File has not been modified\n");
            exit(-1);
        }

        int retval = -1;
        unsigned int curr_user = getuid();

        for (int i = 0; i < file.users; i++){
            if(file.UID[i] == curr_user)
                retval = file.modifications[i];
            // printf("You have modified the file %u times\n", file->modifications[i]);
        }

        exit(retval);

        return retval;
    }

    int log_count = 0;

    array_t *data = user_history_init();
    user_history_t *user_history = (user_history_t *)data->data;

    for (int i = 0; i < data->size; i++){
        if (user_history[i].strikes >= _MAX_STRIKE_){
            printf("User %d has been banned\n", user_history[i].UID);
        }
    }

    // for (int i = 0; i < user_history; i++){
    //     for (int j = 0; j < history[i].strikes; j++){
    //         printf("User %d has %d strikes on file %s\n", history[i].UID, history[i].strikes, history[i].path[j]);
    //     }
    // }
    return 0;
    
}