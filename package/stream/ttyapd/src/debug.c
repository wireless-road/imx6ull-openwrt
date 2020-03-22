
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/syscall.h>

#include <syslog.h>

#include "main.h"
#include "debug.h"

#define MAX_BUFFER		128 * 1024

extern config_t config;

void debug(const char *fmt, ...){
	char tempvar[MAX_BUFFER];
	va_list list;
	int fd;
	
	va_start(list,fmt);
#ifdef HAVE_VSNPRINTF
	vsnprintf(tempvar, MAX_BUFFER, fmt, list);
#else
	vsprintf(tempvar, fmt, list);
#endif
	va_end(list);
	switch(config.debug){
		case DEBUG_STDOUT: 
			printf("%d: %s",syscall(SYS_gettid),tempvar);
			fflush(stdout);
			break;
		case DEBUG_LOGFILE:
			fd=open(config.logfile, O_WRONLY | O_APPEND | O_CREAT);
			write(fd,tempvar,strlen(tempvar));
			close(fd);
			break;
		case DEBUG_SYSLOG:
#ifdef BNAME
			openlog(BNAME, 0, LOG_USER);
#else
			openlog("main", 0, LOG_USER);
#endif
			syslog(LOG_NOTICE, tempvar);
			closelog();
			break;
		case DEBUG_SILENT:
		default:
			break;
	}	
}

