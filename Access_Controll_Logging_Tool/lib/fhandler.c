#include "fhandler.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

void Handle(const char *__lib, const char *__func, void * _funcp){
    char *error;

    void* handle = dlopen(__lib, RTLD_LAZY);
        if (!handle){
            fprintf(stderr, "Error: %s\n", dlerror());
            exit(EXIT_FAILURE);
    }

    dlerror();
	
	*(void **) _funcp = dlsym(handle, __func);
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }
    dlclose(handle);
}