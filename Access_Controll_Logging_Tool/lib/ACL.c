#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
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
 * @brief returns the absolute path of a file stream
 * 
 * @param f The file stream
 * @return char* The absolute path of the file stream
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

	char buf[1024] = "";

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

	Handle("libc.so.6", "fopen", &fopen_ptr);										// Get pointer to fopen
	Handle("libc.so.6", "fread", &fread_ptr);										// Get pointer to fread

	unsigned char *hash_key;
	int action_denied = 0;

	access_t access_type = (access(path, F_OK) == 0) ? __OPEN : __CREATION; 		// Check if file exists

	FILE* fp = fopen_ptr(path, mode);												// Open file

	if (errno == EACCES){
		hash_key = Hash_string("");
		action_denied = 1;

		__abs_path = (char*)malloc(sizeof(char)*254);							
		getcwd(__abs_path, 253);													// Get current working directory
		strcat(__abs_path, "/");													// Append "/" to the end of the path
		strcat(__abs_path, path);													// Append the path to the end of the working directory
	} else {
		hash_key = Hash(fp);														// Get hash_key of file
		__abs_path = get_path(fp);													// Get absolute path of file
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

	if (ret_val != nmemb && nmemb != 0)												// If the write operation was not successful
		action_denied = 1;															// Set action_denied to 1
	else
		__seek_pointer = ftell(stream);												// Save seek pointer

	MD5_CTX md5;
	MD5_Init(&md5);

	fseek(stream, 0, SEEK_SET);														// Set file pointer to the beginning of the file

	hash_key = Hash(stream);														// Get hash_key of file

	fseek(stream, __seek_pointer, SEEK_SET);										// Set seek pointer to original position		

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
	
	long int __seek_pointer = ftell(stream);										// Save seek pointer

	if (ret_val != nmemb && nmemb != 0)												// If the read operation was not successful
		action_denied = 1;															// Set action_denied to 1

	MD5_CTX md5;				
	MD5_Init(&md5);

	fseek(stream, 0, SEEK_SET);														// Set file pointer to the beginning of the file

	hash_key = Hash(stream);														// Get hash_key of file

	fseek(stream, __seek_pointer, SEEK_SET);										// Set seek pointer to original position

	create_log(get_path(stream), __READ, action_denied, hash_key);					// Create and print log entry onto log.txt

	free(hash_key);

	return ret_val;																	// Return the value returned by fread
}
#endif