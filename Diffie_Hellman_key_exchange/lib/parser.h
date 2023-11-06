

typedef struct {
    char *command;
    void *params;
    void *(*func)(void* param);

    int includedParam;
    int NumericParam;
} command;

void parse(int argc, char *argv[], command *commands[]);
