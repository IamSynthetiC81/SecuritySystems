#ifndef _LOG_H_
	#define _LOG_H_
#endif

#include <time.h>
#include "fhandler.h"

#define _LOG_FILE_PATH_ "etc/log.txt"

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

// /**
//  * @brief The log entry structure.
//  * 
//  * @param UID The user ID of the user who performed the action.
//  * @param path The absolute path of the file.
//  * @param timestamp The date and time of the action.
//  * @param access The access type. see access_t
//  * @param action_denied 1 if the action was denied, else 0.
//  * @param fingerprint The fingerprint of the file.
// */
// typedef struct log_entry
// {
//     const unsigned int UID;
//     const char *path;
//     const struct tm timestamp;
//     const access_t access;
//     const int action_denied;
//     const unsigned char fingerprint[16];
// } log_t;

typedef struct log_entry{
    const unsigned int UID;
    const char *path;
    const struct tm timestamp;
    const access_t access;
    const int action_denied;
    const char fingerprint[33];
} logf_t;

/**
 * @brief Create a timestamp object with the current date and time.
 * 
 * @return struct tm The current date and time.
*/
struct tm date_and_time();

/**
 * @brief Prints the log entry to the log file.
 * 
 * @param log_entry The log entry to be printed.
*/
void print_log_to_file(logf_t log_entry);


/**
 * @brief Creates a log entry.
 * 
 * @param path The absolute path of the file.
 * @param access The access type. see access_t
 * @param action_denied 1 if the action was denied, else 0.
 * @param fingerprint The fingerprint of the file.
*/
logf_t create_log(const char *path, const access_t access, const int action_denied, char fingerprint[33]);