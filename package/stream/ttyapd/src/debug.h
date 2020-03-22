#ifndef INC_DEBUG
#define INC_DEBUG

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum {
	DEBUG_STDOUT,
	DEBUG_LOGFILE,
	DEBUG_SYSLOG,
	DEBUG_SILENT,
}debug_t;

void debug(const char *fmt,...);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef INC_DEBUG */
