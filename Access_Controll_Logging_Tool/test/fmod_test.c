#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include "../lib/Misc.h"
#include "../lib/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifdef DEBUG 
	#define printd(format, ...) printf(format, ##__VA_ARGS__)
	#define printld(format, ...) printf("[%s line : %-3d] " format,__FILE__, __LINE__, ##__VA_ARGS__)
#else
	#define printd(format, ...)
	#define printld(format, ...)
#endif

#ifdef VERBOSE
	#define printv(format, ...) if(_VERBOSE_) printf(format, ##__VA_ARGS__)
#else
	#define printv(format, ...)
#endif

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

char *chatGPT_is_a_poet[] = {
        "In the kingdom of code, where bits and bytes play,",
        "C and Unix dance in a code ballet.",
        "With a syntax so terse, and a power so grand,",
        "They conquer the land of the digital sand.",
        " ",
        "In the shell's embrace, Unix takes the lead,",
        "Commands like wizards fulfilling every need.",
        "\"ls\" to see, \"cp\" to copy,",
        "In the command-line dance, it's never floppy.",
        " ",
        "C, the language of pointers and structs,",
        "In the coding arena, it truly constructs.",
        "Memory magic, with a pointer's grace,",
        "In the world of C, it's a memory chase.",
        " ",
        "In the dungeons of code, bugs may creep,",
        "But C and Unix, a vigil they keep.",
        "Grep and sed, searching far and wide,",
        "Fixing errors with a programmer's pride.",
        " ",
        "The pipes connect, like a Unixian dream,",
        "Commands combined, a powerful stream.",
        "Awk and Perl join the coding parade,",
        "In the syntax symphony, they're not afraid.",
        " ",
        "On the threads of execution, C takes a ride,",
        "Multithreading magic, no need to hide.",
        "Mutex and semaphore, in harmony sing,",
        "In the Unix realm, code is the king.",
        " ",
        "In the terminal's glow, where hackers dwell,",
        "C and Unix stories, they love to tell.",
        "A fork in the road, processes divide,",
        "In the Unix tale, where legends bide.",
        " ",
        "So here's to C, to Unix, and the geeky cheer,",
        "In the land of code, where logic is clear.",
        "May your bugs be few, and your code run sublime,",
        "In the Unix-C world, where we rhyme.",
        " ",
        "\t\t~ChatGPT",
    };


int Write(){
    for(int i = 0; i < 41; i++){
        FILE *fp = fopen("test/.testfiles/user_read_write.c", "a");
        if (fp == NULL){
            perror("fopen");
            return 0;
        }


        printd("[%-3d] Write to file\n", __LINE__);
        array_t *data = file_history_init();
        file_history_t *history = (file_history_t *)data->data;

        int mod_before = 0, mod_after = 0, found_file = 0;
        printld("data->size = %d\n", data->size);

        for (int i = 0; i < data->size; i++){
            // printf("[%02d] %s\n", i, history[i].path);
            if (strcmp(history[i].path, "/home/marios/SecuritySystems/Access_Controll_Logging_Tool/test/.testfiles/user_read_write.c") == 0){
                printld("Found file\n");
                for (int j = 0; j < history[i].users; j++){
                    mod_before += history[i].modifications[j];
                }
            }
        }
        printf("[%d]Access number before write = %d\n" ,i, mod_before);

        // fwrite(chatGPT_is_a_poet[i], sizeof(char), strlen(chatGPT_is_a_poet[i]), fp);
        // fwrite("\n", sizeof(char), 1, fp);
        fclose(fp);

        // free(data);
        // printd("[%-3d] Free data\n", __LINE__);
        // data = file_history_init();
        // printf("[%d] data->size = %d\n", __LINE__, data->size);
        // history = (file_history_t *)data->data;

        // for (int i = 0; i < data->size; i++){
        //     if (strcmp(history[i].path, "/home/marios/SecuritySystems/Access_Controll_Logging_Tool/test/.testfiles/user_read_write.c") == 0){
        //         for (int j = 0; j < history[i].users; j++){
        //             mod_after += history[i].modifications[j];
        //         }
        //     }
        // }
        // printf("[%d]Access number after write = %d\n" ,i, mod_after);

        // if (mod_after - 2 != mod_before ){
        //     return 0;
        // }
    }

    return 1;
}

int main(void){
    int retval = Write();

    printf("%-20s : ", "Modification Monitor");
    printf("[%4s]\n", retval ? GRN "PASS" RESET : RED "FAIL" RESET);   
}