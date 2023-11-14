#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>
    
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

char paths[][256] = {
    "test/.testfiles/user_read.c",
    "test/.testfiles/group_read.c",
    "test/.testfiles/other_read.c",
    "test/.testfiles/user_write.c",
    "test/.testfiles/group_write.c",
    "test/.testfiles/other_write.c",
    "test/.testfiles/user_execute",
    "test/.testfiles/group_execute",
    "test/.testfiles/other_execute",
    "test/.testfiles/user_read_write.c",
    "test/.testfiles/group_read_write.c",
    "test/.testfiles/other_read_write.c",
    "test/.testfiles/user_read_execute",
    "test/.testfiles/group_read_execute",
    "test/.testfiles/other_read_execute",
    "test/.testfiles/user_write_execute",
    "test/.testfiles/group_write_execute",
    "test/.testfiles/other_write_execute"
};

int testWrite(){
    for(int i = 0; i < 18; i++){
        FILE *fp = fopen(paths[i], "w");
        if (errno == EACCES) {;
            if (i == 3 || i == 9 || i == 15){
                return 0;
            }
        } else {
            fclose(fp);
            clearerr(fp);
        }
        errno = 0;
    }
    return 1;
}

int testRead(){
    for(int i = 0; i < 18; i++){
        FILE *fp = fopen(paths[i], "r");
        if (errno == EACCES) {
            if (i == 0 || i == 9 || i == 12){
                return 0;
            }
        } else {
            fclose(fp);
            clearerr(fp);
        }
        errno = 0;
    }
    return 1;
}

int testExec(){
    for(int i = 0; i < 18; i++){
        char path[256] = "./";
        strcat(path, paths[i]);
        strcat(path, " ");
        strcat(path, paths[i]);
        strcat(path, " > /dev/null 2>&1");

        

        int exec = system(path);
        if (exec != 0 && (i == 6 || i == 12))
            return 0;
    }
    return 1;
}


int main() {
    printf("%-5s : ", "Write");
    printf("[%4s]\n", testWrite() ? GRN "PASS" RESET : RED "FAIL" RESET);
    
    printf("%-5s : ", "Read");
    printf("[%4s]\n", testRead() ? GRN "PASS" RESET : RED "FAIL" RESET);

    printf("%-5s : ", "Exec");
    printf("[%4s]\n", testExec() ? GRN "PASS" RESET : RED "FAIL" RESET);
}