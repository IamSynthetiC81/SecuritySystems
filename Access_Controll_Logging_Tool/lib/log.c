#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>

#include "fhandler.h"

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
		printf("Error opening log file\n");
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

	return;
}

logf_t create_log(const char *path, const access_t access, const int action_denied, char fingerprint[33]){
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

	return log_entry;
}
