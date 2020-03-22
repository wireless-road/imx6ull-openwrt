#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "debug.h"
#include "main.h"
#include "signal.h"
#include "libiw.h"
#include "serial.h"
#include "jsparse.h"
#include "sys_uci.h"

#define TTY_READ_MAXBUF		4095

config_t config;

void show_usage(){
	printf("USAGE:\n"
		"\t -f |--foreground                         Run in foreground, do not daemonize.\n"
		"\t -l |--logfile <logfile>                  Output Log to logfile. If not set use syslog.\n"
		"\n"
		"\t -i |--interface <wireless-interface>     Wireless interface to track.\n"
		"\t -t |--tty <tty-interface>                TTY interface to listen commands and send replies.\n"
		"\n"
		"\t -h |--help                               Output this message.\n"
		"\n"
		"Version: %s-%s\n\n", VERSION, VDATE
	);
}

void args_work(int argc,char **argv){
	config.daemon = 1;
	config.debug = DEBUG_STDOUT;
#define IS_EQ_DOUBLE_ARG(var, short, long)	( (!strncmp(var, short, sizeof(short))) || (!strncmp(var, long, sizeof(long))) )
	while(argc>1){
		if(IS_EQ_DOUBLE_ARG(argv[1], "-h", "--help")){
			show_usage();
			exit(0);
		}
		if(IS_EQ_DOUBLE_ARG(argv[1], "-f", "--foreground")){
			config.daemon=0;
		}
		if(IS_EQ_DOUBLE_ARG(argv[1], "-l ", "--logfile")){
			config.logfile = calloc(strlen(argv[2])+1,sizeof(unsigned char));
			strcpy(config.logfile, argv[2]);
			config.debug=DEBUG_LOGFILE;
			argc--;argv++;
		}
		if(IS_EQ_DOUBLE_ARG(argv[1], "-i", "--interface")){
			config.wlan_iface = calloc(strlen(argv[2]) + 1, sizeof(unsigned char));
			strcpy(config.wlan_iface, argv[2]);
			argc--;argv++;
		}
		if(IS_EQ_DOUBLE_ARG(argv[1], "-t", "--tty")){
			config.tty_iface = calloc(strlen(argv[2]) + 1, sizeof(unsigned char));
			strcpy(config.tty_iface, argv[2]);
			argc--;argv++;
		}

		argc--;argv++;
	}
#undef IS_EQ_DOUBLE_ARG
}

int check_config(config_t *c){
	int rc = 0;
	
	if(config.tty_iface == NULL || !strlen(config.tty_iface)){
		debug("MAIN: TTY interface not set. Bailing.\n");
		return 1;
	}
	if(config.wlan_iface == NULL || !strlen(config.wlan_iface)){
		debug("MAIN: WLAN interface not set Bailing.\n");
		return 1;
	}
	
	if(config.pidfile == NULL){
		debug("MAIN: Pidfile not set. Using '%s'\n", DEFAULT_PIDFILE);
		config.pidfile = calloc(sizeof(DEFAULT_PIDFILE)+1,sizeof(unsigned char));
		snprintf(config.pidfile, sizeof(DEFAULT_PIDFILE), DEFAULT_PIDFILE);
	}
	
	return rc;
}

void m_pidwrite(void){
	pid_t mypid;
	FILE *pf;
	
	unlink(config.pidfile);
	mypid = getpid();
	pf=fopen(config.pidfile,"w");
	if(pf){
		fprintf(pf,"%d", mypid);
		fclose(pf);
		chmod(config.pidfile, S_IRUSR | S_IRGRP | S_IROTH |
					S_IWUSR | S_IWGRP | S_IWOTH);
	}
}

void m_pidrm(void){
	if(config.pidfile!=NULL){
		if(!access(config.pidfile, W_OK)){
			debug("MAIN: Deleting '%s'\n", config.pidfile);
			unlink(config.pidfile);
		}
	}
}

void * lm_free(void *p){
	if(p!=NULL)
		free(p);
	return NULL;
}

void m_atexit(void){
	m_free(config.pidfile);
	m_free(config.logfile);
	m_free(config.tty_iface);
	m_free(config.wlan_iface);
	libiw_end(&config.iwptr);
	serial_close(config.ttyfd);
	m_pidrm();
}


void m_daemonize(){
	chdir("/");

	fclose(stdout);
	fclose(stderr);
	fclose(stdin);

	pid_t pid=fork();
	switch(pid){
		case 0: /* Child continues to work */
			break;
		case -1: /* Some error */
			debug("MAIN. %s. Fork error. (%s)\n", __func__, strerror(errno));
			exit(1);
			break;
		default: /* Parent exits */
			exit(0);
			break;
	}
}



void m_worker(){
	int rlen;
	unsigned char *buf, *res, *arg, ttybuf[TTY_READ_MAXBUF + 1];
	cmd_t cmd;
	
	config.ttyfd = serial_opentty(config.tty_iface);
	if(config.ttyfd < 0)
		return;
	
	while(libiw_init(config.wlan_iface, &config.iwptr)){
		debug("MAIN: No interface available, waiting to occur '%s'.\n", config.wlan_iface);
		sleep(1);
	}
	
	debug("MAIN: m_worker(). After init all fine.\n");
	while(1){
		memset(ttybuf, 0, TTY_READ_MAXBUF + 1);

		rlen = serial_read(config.ttyfd, ttybuf, TTY_READ_MAXBUF, 5);

		if(rlen > 0){
			cmd = jsparse_get_cmd(ttybuf, &arg);
			switch (cmd) {
				case CMD_STATUS:
					libiw_status(config.iwptr, &buf);
					serial_write(config.ttyfd, buf, strlen(buf));
					m_free(buf);
					break;
				case CMD_SCAN:
					libiw_scan(config.iwptr, &buf);
					serial_write(config.ttyfd, buf, strlen(buf));
					m_free(buf);
					break;
				case CMD_GET_ESSID:
					if (!sys_uci_get("wireless.@wifi-iface[0].ssid", &res)){
						buf = jsparse_make_json_result(res);
						serial_write(config.ttyfd, buf, strlen(buf));
						m_free(buf);
						m_free(res);
					}
					break;
				case CMD_GET_ENCTYPE:
					if (!sys_uci_get("wireless.@wifi-iface[0].encryption", &res)){
						buf = jsparse_make_json_result(res);
						serial_write(config.ttyfd, buf, strlen(buf));
						m_free(buf);
						m_free(res);
					}
					break;
				case CMD_GET_ENCKEY:
					if (!sys_uci_get("wireless.@wifi-iface[0].key", &res)){
						buf = jsparse_make_json_result(res);
						serial_write(config.ttyfd, buf, strlen(buf));
						m_free(buf);
						m_free(res);
					}
					break;
				case CMD_SET_ESSID:
					if(arg && !sys_uci_set("wireless.@wifi-iface[0].ssid", arg)){
						buf = jsparse_make_json_result("ok");
						serial_write(config.ttyfd, buf, strlen(buf));
					}else{
						buf = jsparse_make_json_result("fail");
						serial_write(config.ttyfd, buf, strlen(buf));
					}
					m_free(buf);
					break;
				case CMD_SET_ENCTYPE:
					if(arg && !sys_uci_set("wireless.@wifi-iface[0].encryption", arg)){
						buf = jsparse_make_json_result("ok");
						serial_write(config.ttyfd, buf, strlen(buf));
					}else{
						buf = jsparse_make_json_result("fail");
						serial_write(config.ttyfd, buf, strlen(buf));
					}
					m_free(buf);
					break;
				case CMD_SET_ENCKEY:
					if(arg && !sys_uci_set("wireless.@wifi-iface[0].key", arg)){
						buf = jsparse_make_json_result("ok");
						serial_write(config.ttyfd, buf, strlen(buf));
					}else{
						buf = jsparse_make_json_result("fail");
						serial_write(config.ttyfd, buf, strlen(buf));
					}
					m_free(buf);
					break;
				case CMD_COMMIT:
					sys_uci_commit("wireless.@wifi-iface[0]");
					sys_wifireset();
					buf = jsparse_make_json_result("ok");
					serial_write(config.ttyfd, buf, strlen(buf));
					m_free(buf);
					break;
				default:
					debug("Bogus receive on TTY: '%s'\n", ttybuf);
					break;
			}
			m_free(arg);
		}
		
	}
}

int main(int argc, char ** argv){
	
	memset(&config, 0, sizeof(config_t));
	
	atexit(m_atexit);
	signal_set();
	
	args_work(argc, argv);
	
	if(check_config(&config)){
		debug("\n");
		show_usage();
		m_atexit();
		return -1;
	}
	
	if(config.daemon)
		m_daemonize();

	m_pidwrite();

	m_worker();

	sleep(1);
	return 0;
}
