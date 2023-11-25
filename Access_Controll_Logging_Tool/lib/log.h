#ifndef _LOG_H_
	#define _LOG_H_


#include <time.h>
#include "fhandler.h"
#include "Misc.h"

/**
 * @brief Handles all operations regarding the log file.
 * 
 * 
 */

#define _LOG_FILE_PATH_ "etc/log.txt"

/**
 * @brief The size of the log file. Updated on every log entry and on every read.
*/
size_t _size_ = 0;

/**
 * @brief The access types.
 * 
 * @param __CREATION The file was created.
 * @param __OPEN The file was opened.
 * @param __WRITE The file was written to.
 * @param __READ The file was read from.
 * @param __CLOSE The file was closed.
*/
typedef enum access_types{
	__CREATION,
	__OPEN,
	__WRITE,
	__READ,
    __CLOSE
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

/**
 * @brief The log entry in strings format
 * 
 * @param UID The UID of the user.
 * @param path The absolute path of the file.
 * @param timestamp The timestamp of the log entry.
 * @param access The access type. see access_t
 * @param action_denied 1 if the action was denied, else 0.
 * @param fingerprint The fingerprint of the file.
 * 
*/
typedef struct log_entry_s{
	char UID[sizeof(123456789)];
	char path[256];
	char timestamp[sizeof("2024-01-01 00:00:00")];
	char access[6];
	char action_denied[1];
	char fingerprint[33];
}logs_t;

/**
 * @brief User history in the log. Contains all files the user attempted ro access or modify but was rejected.
 * 
 * @param UID The UID of the user.
 * @param strikes The number of strikes the user has.
 * @param path The absolute path of the files accessed or modified.
*/
typedef struct user_history{
    unsigned int UID;
    int strikes;
    char **path;
} user_history_t;

/**
 * @brief File history in the log. Contains logs regarding a file.
 * 
 * @param path The absolute path of the file.
 * @param UID The UID of the users that accessed or modified, or attempted to, the file.
 * @param modifications The number of times the file was modified by each user.
 * @param users The number of users that modified the file
 * 
*/
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

/**
 * @brief Reads the log file and returns an array of @link user_history_t @endlink in the form of @link array_t @endlink.
 * 
 * @return array_t* The array of @link user_history_t @endlink.
*/
array_t *user_history_init();


/**
 * @brief Reads the log file and returns an array of @link file_history_t @endlink in the form of @link array_t @endlink.
 * 
 * @return array_t* The array of @link file_history_t @endlink.
*/
array_t *file_history_init();

#endif