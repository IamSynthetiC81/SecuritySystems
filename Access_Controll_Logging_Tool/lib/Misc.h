#ifndef _MISC_H_
    #define _MISC_H_


typedef struct array_1d{
    void* data;
    int size;
} array_t;

typedef struct array_2d{
    void* data_1;
    void* data_2;
    int size_1;
    int size_2;
} array2d_t;

typedef struct array_3d{
    void* data_1;
    void* data_2;
    void* data_3;
    int size_1;
    int size_2;
    int size_3;
} array3d_t;

typedef struct array_4d{
    void* data_1;
    void* data_2;
    void* data_3;
    void* data_4;
    int size_1;
    int size_2;
    int size_3;
    int size_4;
} array4d_t;

#endif