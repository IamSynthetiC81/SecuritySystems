#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif

/**
 * @brief Handles the dynamic linking of the functions.
 * 
 * @param __lib The library name to be linked.
 * @param __func The function name to be linked.
 * @param _funcp The pointer to the function to be linked.
 * 
*/
void Handle(const char *__lib, const char *__func, void *_funcp);