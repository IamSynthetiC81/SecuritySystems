#ifndef _MISC_H_
#define _MISC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG 
	#define printd(format, ...) printf(format, ##__VA_ARGS__)
	#define printld(format, ...) printf("[%s line : %-3d] " format,__FILE__, __LINE__, ##__VA_ARGS__)
#else
	#define printd(format, ...)
	#define printld(format, ...)
#endif

typedef struct array_1d{
    void* data;
    int size;
} array_t;


typedef struct String_array {
    char **data;
    size_t size;
    size_t capacity;
    size_t data_size;
} String_array_t;

String_array_t InitStringArray(size_t _data_size){
    String_array_t arr = {
        .data = NULL,
        .size = 0,
        .capacity = 0,
        .data_size = _data_size
    };
    
    arr.data = (char **)malloc(sizeof(char *) * 1 * _data_size);
    arr.capacity = 1;

    return arr;
}

void PushStringArray(String_array_t *arr, char *str){
    if(arr->size == arr->capacity){
        arr->capacity *= 2;
        arr->data = (char **)realloc(arr->data, sizeof(char *) * arr->capacity * arr->data_size);
    }

    arr->data[arr->size] = (char *)malloc(sizeof(char) * strlen(str));
    strcpy(arr->data[arr->size], str);
    arr->size++;
}

void FreeStringArray(String_array_t *arr){
    for(int i = 0; i < arr->size; i++){
        free(arr->data[i]);
    }
    free(arr->data);
}

char* readStringArray(String_array_t *arr, int _index){
    if (_index < arr->size)
        return arr->data[_index];
    else
        return NULL;
}

int setStringArray(String_array_t *arr, long int _index, char *str){
    if (_index >= arr->size)
        return -1;
    
    arr->data[_index] = (char *)realloc(arr->data[_index], sizeof(char) * strlen(str));
    strcpy(arr->data[_index], str);
    return 1;
    
}

#endif