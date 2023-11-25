#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <errno.h>

#include <unistd.h>
#include <stdarg.h>
#include <openssl/md5.h>

#include "log.h"
#include "fhandler.h"

/**
 * @version 1.1.3
 * @authors mkritikakis, hgeorgakopoulos
 * 
 * This file contains the functions to create and print log entries onto log.txt
 * Hooks for fopen, fread, fwrite and fclose are present and can be enabled by defining the macros __HOPEN, __HREAD, __HWRITE and __HDELETE respectively.
 * @example gcc -D__HOPEN -D__HREAD -D__HWRITE -D__HDELETE -o test test.c lib/ACL.c lib/log.c lib/fhandler.c
 *
 * This file is compiled into a shared object file and is loaded into the process address space of the target program using LD_PRELOAD.
 * @example LD_PRELOAD=./libACL.so <executable>
 * 
 * This file can be preloaded into the system with the following command. This way each file access will be logged.
 * @example LD_PRELOAD=./libACL.so
 * @warning This will log all file accesses made by all programs. Even to pipes and sockets. This can be very noisy and WILL throw errors on tmp files.
 * 
 * @todo make it work systemwide for fun.
 * 
*/

/**
 * @brief returns the absolute path of a file stream
 * 
 * @param f The file stream
 * @return char* The absolute path of the file stream
 * 
 * @warning This function is not tested.
 * 
 * @todo test this function
*/
char* get_path(FILE *f){
	char* buf = (char*)malloc(256*sizeof(char));
	char fnmbuf[sizeof "/prof/self/fd/0123456789"];
    sprintf(fnmbuf,"/proc/self/fd/%d", fileno(f));

    ssize_t nr;
    if((nr=readlink(fnmbuf, buf, 256)) < 0 ) return NULL;
    else buf[nr]='\0';

	return buf;
}

/**
 * @brief Creates a hash key from the contents of a file.
 * 
 * @param fp The file stream
 * @return unsigned char* The hash key
*/
unsigned char *Hash(FILE *fp){
	long int __seek_pointer = ftell(fp);											// Save seek pointer

	size_t (*fread_ptr)(void *, size_t, size_t, FILE*);								
	Handle("libc.so.6", "fread", &fread_ptr);										// Get pointer to fread

	unsigned char *hash_key = (unsigned char*) malloc(sizeof(unsigned char)*MD5_DIGEST_LENGTH);

	MD5_CTX md5;
	MD5_Init(&md5);

	fseek(fp, 0, SEEK_SET);															// Set file pointer to the beginning of the file

	char buf[1024] = "\0";

	int nread;
	do{
		nread = fread_ptr(buf, sizeof(char), 1024, fp);

		MD5_Update(&md5, buf, nread);
	} while (nread > 0);

	MD5_Final(hash_key, &md5);														// Get hash_key of file

	fseek(fp, __seek_pointer, SEEK_SET);											// Set seek pointer to original position

	return hash_key;
}

/**
 * @brief Creates a hash key from a string.
 * 
 * @param str The string
 * @return unsigned char* The hash key
*/
unsigned char *Hash_string(char* str){
	unsigned char *hash_key = (unsigned char*) malloc(sizeof(unsigned char)*MD5_DIGEST_LENGTH);

	MD5_CTX md5;
	MD5_Init(&md5);

	MD5_Update(&md5, str, strlen(str));

	MD5_Final(hash_key, &md5);														// Get hash_key of file

	return hash_key;																// Return hash_key
}

#ifdef __HOPEN

/**
 * @brief fopen function hook.
 * 
 * @param path The path of the file to be opened.
 * @param mode The mode in which the file is to be opened.
 * 
 * @return FILE* The file stream.
*/
FILE *fopen(const char *path, const char *mode){
	FILE *(*fopen_ptr)(const char *, const char *);
	size_t (*fread_ptr)(void *, size_t, size_t, FILE*);

	char *__abs_path;

	Handle("libc.so.6", "fopen", &fopen_ptr);										// Set pointer to fopen
	Handle("libc.so.6", "fread", &fread_ptr);										// Set pointer to fread

	unsigned char *hash_key;
	int action_denied = 0;

	access_t access_type = (access(path, F_OK) == 0) ? __OPEN : __CREATION; 		/* Check if file exists
																						If it does, set access_type to __OPEN
																						Else, set access_type to __CREATION
																					*/
	
	FILE *fp = fopen_ptr(path, mode);												// Open file
	if (errno == EACCES || errno == EPERM || errno == EROFS){						// If the open operation was not successful (permission denied)
		hash_key = Hash_string("");													// Get hash_key of an empty string
		action_denied = 1;															

		__abs_path = (char*)malloc(sizeof(char)*254);							
		getcwd(__abs_path, 253);													// Get current working directory
		strcat(__abs_path, "/");													// Append "/" to the end of the path
		strcat(__abs_path, path);													// Append the path to the end of the working directory
	} else {
		hash_key = Hash(fp);														// Get hash_key of file
		__abs_path = get_path(fp); 													// Get absolute path of file
	}

	create_log(__abs_path, access_type, action_denied, hash_key); 					// Create and pring log entry onto log.txt

	free(hash_key);		
	free(__abs_path);

	return fp;
}
#endif

#ifdef __HWRITE

/**
 * @brief fwrite function hook.
 * 
 * @param ptr The pointer to the data to be written.
 * @param size The size of each element to be written.
 * @param nmemb The number of elements to be written.
 * @param stream The file stream.
 * 
 * @return size_t The number of elements written.
*/
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream){
	size_t (*fread_ptr)(void *, size_t, size_t, FILE*);
	size_t (*fwrite_ptr)(const void *, size_t, size_t, FILE*);

	Handle("libc.so.6", "fread", &fread_ptr);
	Handle("libc.so.6", "fwrite", &fwrite_ptr);

	unsigned char *hash_key;
	int action_denied = 0;
	long int __seek_pointer = 0;
	
	size_t ret_val = fwrite_ptr(ptr, size, nmemb, stream); 							// Write to the file

	fflush(stream);																	/* Flush the file stream
																						This is important so that the IO operation is not delayed
																						by the OS and the changes are hashed and 
																						stored to the log correctly !!!. 
																					*/

	if (errno == EACCES || errno == EPERM || errno == EROFS)						// If the write operation was not successful
		action_denied = 1;															// Set action_denied to 1

	MD5_CTX md5;
	MD5_Init(&md5);

	hash_key = Hash(stream);														// Get hash_key of file

	create_log(get_path(stream), __WRITE, action_denied, hash_key);					// Create and print log entry onto log.txt

	free(hash_key);																	

	return ret_val;																	// Return the value returned by fwrite
}
#endif

#ifdef __HREAD
/**
 * @brief fread function hook.
 * 
 * @param ptr The pointer to the data to be read.
 * @param size The size of each element to be read.
 * @param nmemb The number of elements to be read.
 * @param stream The file stream.
 * 
 * @return size_t The number of elements read.
*/
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream){
	FILE *(*fopen_ptr)(const char *, const char *);									// Get pointer to fopen
	size_t (*fread_ptr)(void *, size_t, size_t, FILE*);								// Get pointer to fread

	Handle("libc.so.6", "fopen", &fopen_ptr);										
	Handle("libc.so.6", "fread", &fread_ptr);

	unsigned char *hash_key;
	int action_denied = 0;

	size_t ret_val = fread_ptr(ptr, size, nmemb, stream);							// Read from the file

	if (ret_val != nmemb && nmemb != 0)												// If the read operation was not successful
		action_denied = 1;															// Set action_denied to 1

	MD5_CTX md5;				
	MD5_Init(&md5);

	fseek(stream, 0, SEEK_SET);														// Set file pointer to the beginning of the file

	hash_key = Hash(stream);														// Get hash_key of file

	create_log(get_path(stream), __READ, action_denied, hash_key);					// Create and print log entry onto log.txt

	free(hash_key);

	return ret_val;																	// Return the value returned by fread
}
#endif

#ifdef __HDELETE
/**
 * @brief fclose function hook.
 * 
 * @param fp The file stream.
 * 
 * @return int 0 on success, EOF on failure.
*/
int fclose(FILE *fp){
	int (*fclose_ptr)(FILE *);

	Handle("libc.so.6", "fclose", &fclose_ptr);

	// printld("\t\t\t\tfclose() : \n");

	return fclose_ptr(fp);
}
#endif