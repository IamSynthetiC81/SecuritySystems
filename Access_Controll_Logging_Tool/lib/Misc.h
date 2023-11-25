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

/**
 * @brief A structure to store a 1D array of any type of data.
 * @attention The data must typecasted to void* before storing. and must be typecasted back to the original type before using.  
 * @param data A pointer to the array.
 * @param size The size of the array.
*/
typedef struct array_1d{
    void* data;
    int size;
} array_t;

/**
 * @brief A structure to store an array of strings
 * 
 * @param data A pointer to the array of strings.
 * @param size The size of the array.
 * @param capacity The capacity of the array.
 * @param data_size The size for each string in the array.
*/
typedef struct String_array {
    char **data;
    size_t size;
    size_t capacity;
    size_t data_size;
} String_array_t;

/**
 * @brief Initialize a the string array.
 * @example String_array_t arr = InitStringArray(256);
 * @param _data_size The size for each string in the array.
 * @return String_array_t
*/
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

/**
 * @brief Push a string to the string array.
 * @example PushStringArray(&arr, "Hello World");
 * @param arr A pointer to the string array.
 * @param str The string to be pushed.
 * @return void
 * 
 * @todo Add a check for the size of the string.
 * @todo return an error code.
*/
void PushStringArray(String_array_t *restrict arr, const char *restrict str){
    if(arr->size == arr->capacity){
        arr->capacity += 1;
        arr->data = (char **)realloc(arr->data, sizeof(char *) * arr->capacity);
        // arr->data = (char **)realloc(arr->data, sizeof(char *) * arr->capacity * arr->data_size);
    }

    arr->data[arr->size] = (char *)malloc(sizeof(char) * arr->data_size);
    strcpy(arr->data[arr->size], str);
    arr->size++;
}



/**
 * @brief read a string from the string array.
 * @example char *str = readStringArray(&arr, 0);
 * @param arr A pointer to the string array.
 * @param _index The index of the string to be read.
 * 
 * @return char* returns the sting pointed to by the @link _index @endlink.
*/
char* readStringArray(String_array_t *arr, unsigned int _index){
    if (_index < arr->size)
        return arr->data[_index];
    else
        return NULL;
}

/**
 * @brief Set a string in the string array.
 * @example setStringArray(&arr, 0, "Hello World");
 * @param arr A pointer to the string array.
 * @param _index The index to the string to be set.
 * @param str The string to be set.
 * 
 * @return int returns -1 when @link _index @endlink is larger than the @link _size @endlink. returns 1 on success.
 * 
*/
int setStringArray(String_array_t *restrict arr , unsigned int _index, const char *restrict str){
    if (_index >= arr->size)
        return -1;
    
    char* clean_string = calloc(arr->data_size, sizeof(char));

    strcpy(arr->data[_index], clean_string);
    strcpy(arr->data[_index], str);
    return 1;
    
}

/**
 * @brief Free the string array.
 * @example FreeStringArray(&arr);
 * @param arr A pointer to the string array.
*/
void FreeStringArray(String_array_t *arr){
    if (arr == NULL)
        printf("arr is NULL\n");

    free(arr->data);

    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

#endif