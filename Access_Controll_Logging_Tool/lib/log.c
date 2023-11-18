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
	_TIMESTAMP,
	_ACCESS,
	_ACTION_DENIED,
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

	if(fp!=NULL){
		fclose(fp);
	}
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
	FILE* (*fopen_ptr)(const char *, const char *);
	Handle("libc.so.6", "fopen", &fopen_ptr);

	FILE *fp = fopen_ptr(_LOG_FILE_PATH_, "r");
    if (fp == NULL){
        perror("Parse_log : fopen");
        exit(EXIT_FAILURE);
    }


	char **lines = (char**)malloc(sizeof(char *));
	int log_count = 0;

	while (!feof(fp)){
		char buffer[256];
		fgets(buffer, 512, fp);

		if (strlen(buffer) < 10){
			break;
		}

		printld("buffer : %s\n", buffer);

		lines[log_count] = (char *)malloc(sizeof(char) * 256);
		strcpy(lines[log_count], buffer);
		log_count++;
	}
	_size_ = log_count;
	printld("log_count = %d\n", log_count);

	char ***logs = (char ***)malloc(sizeof(char **) * 6);
	logs[_UID] = (char **)malloc(sizeof(char *) * log_count +1 );
	logs[_PATH] = (char **)malloc(sizeof(char *) * log_count + 1);
	logs[_TIMESTAMP] = (char **)malloc(sizeof(char *) * log_count + 1);
	logs[_ACCESS] = (char **)malloc(sizeof(char *) * log_count + 1);
	logs[_ACTION_DENIED] = (char **)malloc(sizeof(char *) * log_count + 1);
	logs[_FINGERPRINT] = (char **)malloc(sizeof(char *) * log_count + 1);

	for(int i = 0; i < log_count; i++){
		char time[9], date[10];

		logs[_UID][i] = (char *)malloc(sizeof(char) * 10);
		logs[_PATH][i] = (char *)malloc(sizeof(char) * 256);
		logs[_TIMESTAMP][i] = (char *)malloc(sizeof(char) * 18);
		logs[_ACCESS][i] = (char *)malloc(sizeof(char) * 6);
		logs[_ACTION_DENIED][i] = (char *)malloc(sizeof(char) * 1);
		logs[_FINGERPRINT][i] = (char *)malloc(sizeof(char) * 33);

		
		sscanf(lines[i], "%s %s | UID : %s | Action : %s | Denied : %c | Fingerprint : %s | Path : %s\n",
		 time, date, logs[_UID][i], logs[_ACCESS][i], logs[_ACTION_DENIED][i], logs[_FINGERPRINT][i], logs[_PATH][i]);

		free(lines[i]);

		strcat(logs[_TIMESTAMP][i], time);
		strcat(logs[_TIMESTAMP][i], " ");
		strcat(logs[_TIMESTAMP][i], date);
	}

	free(lines);

	fclose(fp);

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

	printf("Parsed log\n");

	user_history_t* history = malloc(sizeof(user_history_t) * 1);
	unsigned int users = 0;
	int found_user = 0;

	for (int i = 0; i < _size_; i++){
		found_user = 0;

		for (int j = 0; j < users; j++){
			if (history[j].UID == atoi(logs[i][_UID])){
				printf("found user %d\n", history[j].UID);
				
				history[j].strikes++;
				printf("Strike %d\n", history[j].strikes);

				history[j].path = (char **)realloc(history[j].path, sizeof(char *) * history[j].strikes);
				history[j].path[history[j].strikes-1] = (char *)malloc(sizeof(char) * 256);

				strcpy(history[j].path[history[j].strikes-1], logs[i][_PATH]);
				printd("path %s\n", history[j].path[history[j].strikes - 1]);

				found_user = 1;
				continue;
			}
		}

		if (!found_user){
			printd("user %d not found\n", atoi(logs[i][_UID]));
			printd("users %d\n", users);
			users++;

			history = (user_history_t *)realloc(history, sizeof(user_history_t) * users);
			history[users-1].UID = atoi(logs[i][_UID]);
			history[users-1].strikes = 1;
			history[users-1].path = (char **)malloc(sizeof(char *) * 1);
			history[users-1].path[0] = (char *)malloc(sizeof(char) * 256);
			strcpy(history[users-1].path[0], logs[i][_PATH]);
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

	// printf("About to free\n");

	return array;
}

int add_to_file_history(array_t* source, file_history_t target ){

	file_history_t* _source = (file_history_t*)source->data;

	_source = realloc(_source, (source->size + 1) * sizeof(file_history_t));
	if(_source == NULL){
		exit(0);
		return 0;
	}
	_source[source->size + 1] = target;

	return 1;
}

array_t *file_history_init(){
	char ***logs = parse_log();
	printld("log size : %lu\n", _size_);

	file_history_t *history = (file_history_t *)malloc(sizeof(file_history_t) * 0);
	char **state = (char**)malloc(sizeof(char*) * 1);
	int files = 0;
	int found_file = 0;
	int found_user = 0;

	for (int i = 0; i < _size_; i++ ){
		found_file = 0;
		
		for (int j = 0; j < files; j++){
			if (strcmp(history[j].path, logs[_PATH][i]) == 0){
				found_file = 1;

				printd("[%-3d]found file %s\n",__LINE__, history[j].path);

				if(strcmp(state[j], logs[_FINGERPRINT][i]) == 0){
					printd("\t[%-3d]state is the same\n",__LINE__);
					continue;
				}

				printd("\t[%-3d]state is different\n", __LINE__);

				for(int k = 0; k < history[j].users; k++){
					if (history[j].UID[k] == atoi(logs[_UID][i])){
						history[j].modifications[k]++;
						printd("\tUser %d has modified file %s %d times\n", history[j].UID[k], history[j].path, history[j].modifications[k]);

						found_user = 1;

						strcpy(state[j], logs[_FINGERPRINT][i]);

						// printf("user %d has modified file %s %d times\n", history[j].UID[k], history[j].path, history[j].modifications[k]);
						continue;
					}
				}

				if (!found_user){
					printd("\t[%-3d]user %d not found\n",__LINE__, atoi(logs[_UID][i]));

					history[j].users = 1;
					history[j].UID = (unsigned int *)realloc(history[j].UID, sizeof(unsigned int) * history[j].users);
					history[j].modifications = (unsigned int *)realloc(history[j].modifications, sizeof(unsigned int) * history[j].users);

					history[j].UID[history[j].users-1] = atoi(logs[_UID][i]);
					history[j].modifications[history[j].users-1] = 1;


					strcpy(state[j], logs[_FINGERPRINT][i]);
				}
				
				found_user = 0;

				// printf("User %d has modified file %s %d times\n", history[j].UID[history[j].users - 1], history[j].path, history[j].modifications[history[j].users - 1]);

				continue;
			}
		}

		if (!found_file){
			files++;
			printld("Files = %d\n", files);
			history = (file_history_t *)realloc(history, sizeof(file_history_t) * (files+1));
			history[files-1].path = (char *)malloc(sizeof(char) * 256);
			strcpy(history[files-1].path, logs[_PATH][i]);

			// printf("File %s\n", history[files - 1].path);

			history[files-1].UID = (unsigned int *)malloc(sizeof(unsigned int) * 1);
			history[files-1].modifications = (unsigned int *)malloc(sizeof(unsigned int) * 1);

			history[files-1].UID[0] = atoi(logs[_UID][i]);
			history[files-1].modifications[0] = 1;

			history[files-1].users = 1;

			state = (char **)realloc(state, sizeof(char *) * (files+1));
			state[files-1] = (char *)malloc(sizeof(char) * 33);
			strcpy(state[files-1], logs[_FINGERPRINT][i]);

			printld("Created file entry\n");
			// printf("User %d has modified file %s %d times\n", history[files-1].UID[0], history[files-1].path, history[files-1].modifications[0]);
		}
	}

	array_t *array = (array_t *)malloc(sizeof(array_t) * 1);
	array->data = (void *)history;
	array->size = files;

	free(state);
	free(logs);

	return array;
}