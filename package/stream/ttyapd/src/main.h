#ifndef INC_MAIN
#define INC_MAIN

#include <pthread.h>

#include "debug.h"

#ifndef VERSION
#define VERSION		"0.1"
#endif
#define DEFAULT_PIDFILE		"/var/run/" BNAME ".pid"

typedef struct {
	unsigned char		daemon;
	unsigned char		*pidfile;
	debug_t				debug;
	unsigned char		*logfile;
	
	unsigned char		*tty_iface;
	unsigned char		*wlan_iface;
	void				*iwptr;
	int					ttyfd;
}config_t;


void m_atexit(void);
void *lm_free(void *p);
#define m_free(p) do{p = lm_free(p);}while(0);

#endif /* #ifndef INC_MAIN */
