#ifndef _LOG_H_
	#define _LOG_H_
#endif

#include <time.h>

#define _LOG_FILE_PATH_ "etc/log.txt"

typedef enum access_types{
	__CREATION,
	__OPEN,
	__WRITE,
	__READ
} access_t;

typedef struct log_entry{
    const unsigned int UID;
    const char *path;
    const struct tm timestamp;
    const access_t access;
    const int action_denied;
    unsigned char fingerprint[16];
} log_t;

void Handle(const char *__lib, const char *__func, void *_funcp);

struct tm date_and_time();

void print_log_to_file(log_t log_entry);

log_t create_log(const char *path, const access_t access, const int action_denied, unsigned char fingerprint[16]);