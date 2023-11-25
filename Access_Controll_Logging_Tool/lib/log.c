#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>

#include "fhandler.h"
#include "Misc.h"

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

enum logs{
	_UID,
	_PATH,
	_FINGERPRINT
};

struct tm date_and_time(){
	time_t rawtime = time(NULL);
	struct tm *timestamp = localtime(&rawtime);

	timestamp->tm_year += 1900;
	timestamp->tm_mon += 1;

	return *timestamp;
}

void print_log_to_file(logf_t log_entry){
	FILE *(*fopen_ptr)(const char *, const char *);
	Handle("libc.so.6", "fopen", &fopen_ptr);

	size_t (*fwrite_ptr)(const void *, size_t, size_t, FILE*);
	Handle("libc.so.6", "fwrite", &fwrite_ptr);
	
	FILE *fp = fopen_ptr(_LOG_FILE_PATH_, "a");
	if(fp == NULL){
		// perror("print_log_to_file : fopen");
		if(errno == EACCES){
			printf("Permission denied\n");
		} else if(errno == ENOENT){
			printf("File does not exist\n");
		} else if (errno == EROFS){
			printf("Read only file system\n");
		} else {
			printf("Unknown error\n");
		}
		exit(EXIT_FAILURE);
	}


	struct tm asd = log_entry.timestamp;
	asd.tm_year += 1900;

	int hour = log_entry.timestamp.tm_hour;
	int min = log_entry.timestamp.tm_min;
	int sec = log_entry.timestamp.tm_sec;

	int day = log_entry.timestamp.tm_mday;
	int month = log_entry.timestamp.tm_mon;
	int year = log_entry.timestamp.tm_year;

	char *access;
	if (log_entry.access == __CREATION){
		access = "CREATE";
	} else if (log_entry.access == __OPEN){
		access = "OPEN";
	} else if (log_entry.access == __WRITE){
		access = "WRITE";
	} else {
		access = "READ";
	}

	char log1[120];
	for (int i = 0; i < 120; i++){
		log1[i] = '\0';
	}

	snprintf(log1,85 ,"[%02d:%02d:%02d] %02d/%02d/%04d | UID : %5d | Action : %6s | Denied : %1d | Fingerprint : ", hour, min, sec , day, month, year, log_entry.UID, access, log_entry.action_denied);
	fwrite_ptr(log1, sizeof(char), 83, fp);
	
	char log2[50] = "";
	char key[50] = "";

	for (int i = 0; i < 16; i++){
        snprintf(log2, 3, "%02x", log_entry.fingerprint[i]);
        strcat(key, log2);		
	}
	fwrite_ptr(key, sizeof(char), 32, fp);

	snprintf(log2, strlen(log_entry.path) + 12, " | Path : %s\n", log_entry.path);
	fwrite_ptr(log2, sizeof(char), strlen(log2), fp);

	fflush(fp);

	fclose(fp);
	
}

logf_t create_log(const char *path, const access_t access, const int action_denied, unsigned char fingerprint[33]){
	logf_t log_entry = {
		.UID = getuid(),
		.path = path,
		.timestamp = date_and_time(),
		.access = access,
		.action_denied = action_denied,
		.fingerprint = {
			*fingerprint = fingerprint[0],
			*fingerprint = fingerprint[1],
			*fingerprint = fingerprint[2],
			*fingerprint = fingerprint[3],
			*fingerprint = fingerprint[4],
			*fingerprint = fingerprint[5],
			*fingerprint = fingerprint[6],
			*fingerprint = fingerprint[7],
			*fingerprint = fingerprint[8],
			*fingerprint = fingerprint[9],
			*fingerprint = fingerprint[10],
			*fingerprint = fingerprint[11],
			*fingerprint = fingerprint[12],
			*fingerprint = fingerprint[13],
			*fingerprint = fingerprint[14],
			*fingerprint = fingerprint[15]
		}
	};

	print_log_to_file(log_entry);

	_size_++;

	return log_entry;
}

char ***parse_log(){
	printld("parse_log() : Starting\n");
	FILE* (*fopen_ptr)(const char *, const char *);
	Handle("libc.so.6", "fopen", &fopen_ptr);

	FILE *fp = fopen_ptr(_LOG_FILE_PATH_, "r");
    if (fp == NULL){
        perror("Parse_log : fopen");
        exit(EXIT_FAILURE);
    }


	char **lines = (char**)malloc(sizeof(char *));
	int log_count = 0;

	printld("parse_log() : Reading file\n");
	fseek(fp, 0, SEEK_SET);
	while (!feof(fp)){
		printld("log_count = %d\n", log_count);
		char buffer[512];
		fgets(buffer, 512, fp);

		if (strlen(buffer) < 10)
			break;

		lines = (char **)realloc(lines, sizeof(char *) * (log_count + 1));
		lines[log_count] = (char *)malloc(sizeof(char) * 512);

		strcpy(lines[log_count], buffer);
		log_count++;
	}
	printld("parse_log() : Read %d lines\n", log_count);
	fclose(fp);

	_size_ = log_count;

	printld("parse_log() : Allocating memory\n");

	char **UID = (char **)malloc(sizeof(char*) * log_count);
	char **PATH = (char **)malloc(sizeof(char*) * log_count);
	char **FINGERPRINT = (char **)malloc(sizeof(char*) * log_count);

	printld("parse_log() : Allocated memory\n");

	for(int i = 0; i < log_count; i++){

		UID[i] = (char *)malloc(sizeof(char) * 10);
		PATH[i] = (char *)malloc(sizeof(char) * 256);
		FINGERPRINT[i] = (char *)malloc(sizeof(char) * 33);

		sscanf(lines[i], "[%*d:%*d:%*d] %*d/%*d/%*d | UID : %s | Action : %*s | Denied : %*d | Fingerprint : %s | Path : %s\n"
		, UID[i],FINGERPRINT[i], PATH[i]);

		free(lines[i]);
	}

	printld("parse_log() : Freeing lines\n");

	free(lines);

	char ***logs = (char ***)malloc(sizeof(char **) * 3);
	logs[_UID] = UID;
	logs[_PATH] = PATH;
	logs[_FINGERPRINT] = FINGERPRINT;

	printld("parse_log() : Returning\n");

	return logs;
}

int string_included(user_history_t history, const char *path){
    for (int j = 0; j < history.strikes; j++){
        if (strcmp(history.path[j], path) == 0){
            return 1;
        }
    }
    return 0;
}

array_t *user_history_init(){
	char ***logs = parse_log();

	user_history_t* history = malloc(sizeof(user_history_t) * 1);
	unsigned int users = 0;
	int found_user = 0;

	for (int i = 0; i < _size_; i++){
		found_user = 0;

		for (int j = 0; j < users; j++){
			if (history[j].UID == atoi(logs[_UID][i])){
				
				history[j].strikes++;

				history[j].path = (char **)realloc(history[j].path, sizeof(char *) * history[j].strikes);
				history[j].path[history[j].strikes-1] = (char *)malloc(sizeof(char) * 256);

				strcpy(history[j].path[history[j].strikes-1], logs[_PATH][i]);
				printd("path %s\n", history[j].path[history[j].strikes - 1]);

				found_user = 1;
				continue;
			}
		}

	

		if (!found_user){
			printd("user %d not found\n", atoi(logs[_UID][i]));
			printd("users %d\n", users);
			users++;

			history = (user_history_t *)realloc(history, sizeof(user_history_t) * users);
			history[users-1].UID = atoi(logs[_UID][i]);
			history[users-1].strikes = 1;
			history[users-1].path = (char **)malloc(sizeof(char *) * 1);
			history[users-1].path[0] = (char *)malloc(sizeof(char) * 256);
			strcpy(history[users-1].path[0], logs[_PATH][i]);
		}
	}

	

	array_t *array = (array_t *)malloc(sizeof(array_t) * 1);
	array->data = (void *)history;
	array->size = users;

	// for (int i = 0; i < 6; i++ ){
	// 	for (int j = 0; j < _size_; j++)
	// 		free(logs[i][j]);	
	// 	free(logs[i]);
	// }

	return array;
}

array_t *file_history_init(){
	printld("file_history_init() : Starting\n");

	char ***logs = parse_log();
	printld("log size : %lu\n", _size_);

	file_history_t *history = (file_history_t *)malloc(sizeof(file_history_t) * 1);

	String_array_t fingerprints = InitStringArray(33);

	int files = 0;
	int found_file = 0;
	int found_user = 0;


	for (int log_index = 0; log_index < _size_; log_index++){
		printld("log_index = %d\n", log_index);
		// search for file in history
		found_file = 0;

		for (int history_index = 0; history_index < files; history_index++){
			printld("\thistory_index = %d\n", history_index);
			printld("\tHistory path = %s\n", history[history_index].path);
			printld("\tLog path = %s\n", logs[_PATH][log_index]);


			if (strcmp(history[history_index].path, logs[_PATH][log_index]) == 0){
				printld("\t\tFound file\n");
				printld("\t\thistory_index = %d\n", history_index);
				
				found_file = 1;

				printld("\t\thistory[history_index].users = %d\n", history[history_index].users);
				printld("\t\tfingerprints.data[history_index] = %s\n", fingerprints.data[history_index]);

				// check if the file has different fingerprint
				if (strcmp(fingerprints.data[history_index], logs[_FINGERPRINT][log_index]) == 0){
					printld("\t\t\tFingerprint is the same\n");
					continue;
				} else{
					printld("\t\t\tFingerprint is different\n");
				}

				// search for user in file history
				found_user = 0;
				printld("\t\tUser index = %d\n", history[history_index].users);

				for (int user_index = 0; user_index < history[history_index].users; user_index++){
					printld("\t\t\tuser_index = %d\n", user_index);
					printld("\t\t\tUID = %d\n", history[history_index].UID[user_index]);
					printld("\t\t\tlog UID = %d\n", atoi(logs[_UID][log_index]));

					if (history[history_index].UID[user_index] == atoi(logs[_UID][log_index])){
						found_user = 1;
						printld("\t\t\t\tFound user\n");
						printld("\t\t\t\tfingerprints.data[user_index] = %s\n", fingerprints.data[user_index]);
						printld("\t\t\t\tlog fingerprint = %s\n", logs[_FINGERPRINT][log_index]);

						// check if the fingerprint has changed for this file
						if (strcmp(fingerprints.data[user_index], logs[_FINGERPRINT][log_index]) != 0){
							printld("\t\t\t\tFingerprint changed\n");
							history[history_index].modifications[user_index] += 1;
							setStringArray(&fingerprints, user_index, logs[_FINGERPRINT][log_index]);
						}
					}
				}

				if (!found_user){
					printld("\t\t\tUser not found\n");
					history[history_index].users += 1;
					history[history_index].UID = (unsigned int *)realloc(history[history_index].UID, sizeof(unsigned int) * history[history_index].users);
					printld("\t\t\thistory[history_index].users = %d\n", history[history_index].users);

					history[history_index].modifications = (unsigned int *)realloc(history[history_index].modifications, sizeof(unsigned int) * history[history_index].users);

					history[history_index].UID[history[history_index].users - 1] = atoi(logs[_UID][log_index]);
					printld("history[history_index].UID[history[history_index].users - 1] = %d\n", history[history_index].UID[history[history_index].users - 1]);
					history[history_index].modifications[history[history_index].users - 1] = 1;
					printld("history[history_index].modifications[history[history_index].users - 1] = %d\n", history[history_index].modifications[history[history_index].users - 1]);
				}
			}
		}

		if (!found_file){
			printld("File not found\n");
			files += 1;
			history = (file_history_t *)realloc(history, sizeof(file_history_t) * files);
			history[files - 1].path = (char *)malloc(sizeof(char) * 256);
			strcpy(history[files - 1].path, logs[_PATH][log_index]);
			history[files - 1].users = 1;
			history[files - 1].UID = (unsigned int *)malloc(sizeof(unsigned int) * 1);
			history[files - 1].modifications = (unsigned int *)malloc(sizeof(unsigned int) * 1);
			history[files - 1].UID[0] = atoi(logs[_UID][log_index]);
			history[files - 1].modifications[0] = 1;

			PushStringArray(&fingerprints, logs[_FINGERPRINT][log_index]);
		}
		printld("Next log\n");
	}

	array_t *array = (array_t *)malloc(sizeof(array_t) * 1);
	array->data = (void *)history;
	array->size = files;

	// for (int i = 0; i < files; i++ ){
	// 	printf("Path = %s\n", history[i].path);
	// 	printf("  Users = %d\n", history[i].users);
	// 	for (int j = 0; j < history[i].users; j++){
	// 		printf("    UID = %d\n", history[i].UID[j]);
	// 		printf("    Modifications = %d\n", history[i].modifications[j]);
	// 	}
	// }

	printld("Freeing logs\n");
	free(logs);

	printld("file_history_init() : Returning\n");

	return array;
}