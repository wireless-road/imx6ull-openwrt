#include <signal.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "main.h"
#include "debug.h"


void sig_ret_handle(int sigval){
	int child_return_value;
	if(sigval == SIGCHLD){
		debug("SIGCHLD received !\n");
		wait(&child_return_value);
	}else if((sigval == SIGTERM)||(sigval == SIGINT)){
		debug("SIGINT or SIGTERM received. Exiting.\n");
		m_atexit();
		exit(EXIT_SUCCESS);
	}else if(sigval == SIGUSR1){
		debug("SIGUSR1 received\n");
		
	}else if(sigval == SIGALRM){
		debug("SIGALRM received\n");
		
	}
}

void signal_set(void){
	signal(SIGCHLD, sig_ret_handle);
	signal(SIGTERM, sig_ret_handle);
	signal(SIGINT, sig_ret_handle);
	signal(SIGUSR1, sig_ret_handle);
	signal(SIGALRM, sig_ret_handle);
}
