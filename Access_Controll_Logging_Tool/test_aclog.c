#include <stdio.h>
#include <stdlib.h>

int main(){
    int exec = system("make test");

    return exec;
}