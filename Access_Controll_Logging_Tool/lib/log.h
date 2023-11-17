#ifndef _LOG_H_
	#define _LOG_H_


#include <time.h>
#include "fhandler.h"
#include "Misc.h"

#define _LOG_FILE_PATH_ "etc/log.txt"

unsigned int _size_ = 0;

/**
 * @brief The access types.
 * 
 * @param __CREATION The file was created.
 * @param __OPEN The file was opened.
 * @param __WRITE The file was written to.
 * @param __READ The file was read from.
*/
typedef enum access_types{
	__CREATION,
	__OPEN,
	__WRITE,
	__READ
} access_t;

/**
 * @brief The log entry.
 * 
 * @param UID The UID of the user.
 * @param path The absolute path of the file.
 * @param timestamp The timestamp of the log entry.
 * @param access The access type. see access_t
 * @param action_denied 1 if the action was denied, else 0.
 * @param fingerprint The fingerprint of the file.
*/
typedef struct log_entry{
    const unsigned int UID;
    const char *path;
    const struct tm timestamp;
    const access_t access;
    const int action_denied;
    const char fingerprint[33];
} logf_t;

typedef struct log_entry_s{
	char UID[sizeof(123456789)];
	char path[256];
	char timestamp[10];
	char access[6];
	char action_denied[1];
	char fingerprint[33];
}logs_t;

typedef struct user_history{
    unsigned int UID;
    int strikes;
    char **path;
} user_history_t;

typedef struct file_history{
    char *path;
    unsigned int *UID;
    unsigned int *modifications;
    int users;
} file_history_t;

/**
 * @brief Creates a log entry.
 * 
 * @param path The absolute path of the file.
 * @param access The access type. see access_t
 * @param action_denied 1 if the action was denied, else 0.
 * @param fingerprint The fingerprint of the file.
*/
logf_t create_log(const char *path, const access_t access, const int action_denied, unsigned char fingerprint[33]);

array_t *user_history_init();

array_t *file_history_init();

#endif