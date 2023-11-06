#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "parser.h"

#define assert(expr, ... ) if (!(expr)) { printf( __VA_ARGS__); exit(1); }

    

void parse(int argc, char *argv[], command *commands[]){
    for (int i = 1; i < argc; i++){
        for (int j = 0; j < argc; j++){
            if (strcmp(argv[i], commands[j]->command) == 0){
                if (commands[j]->includedParam){
                    assert(i + 1 < argc, "Error: %s requires a parameter\n", argv[i]);

                    long num;
                    char *string;

                    num = strtol(argv[++i], &string, 0);

                    if (commands[j]->NumericParam ){
                        assert(num != 0, "Error: %s. Incorrect type\n", argv[i - 1])
                            commands[j]->func((void *)num);
                    }
                    else
                    {
                        assert(*string != '\0', "Error: %s. Empty parameter\n", argv[i - 1]);
                        commands[j]->func((void *)string);
                    }
                }
                else
                {
                    commands[j]->func(NULL);
                }
                break;
            }
        }
    }
}