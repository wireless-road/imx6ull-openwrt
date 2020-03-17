//#define TEST 1 

#ifdef TEST
#define _XOPEN_SOURCE 600
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <sys/poll.h>
#include <pthread.h>

#include "serial.h"
#include "main.h"

pthread_mutex_t serial_mutex;

#define POLL_SET_IN(sfd,pset) do{pset.fd=sfd; pset.events=POLLIN;}while(0);

int serial_opentty(char *ttyname){
	int fd;
	struct termios ttyS;
	int flags;
#ifdef TEST
	fd = posix_openpt(O_RDWR |O_NOCTTY |O_NDELAY);
	grantpt(fd);
	unlockpt(fd);
	debug("Slave tty: '%s'\n", ptsname(fd));
	return fd;
#else
	fd = open(ttyname, O_RDWR | O_NOCTTY |O_NDELAY);

	if (fd == -1){
		debug("Error: Can't open port '%s'\n",ttyname);
		return -1;
	}

	fcntl(fd, F_SETFL, 0);

	tcgetattr(fd, &ttyS);

	cfsetispeed(&ttyS, B115200);
	cfsetospeed(&ttyS, B115200);

	ttyS.c_iflag &= ~(IGNPAR|INPCK|IUCLC|IXANY|IXOFF|IMAXBEL);
	ttyS.c_iflag |= IGNBRK;
	ttyS.c_oflag &= ~(OLCUC|ONLCR|OCRNL|ONOCR|ONLRET|OFILL);

	ttyS.c_cflag &= ~(CSTOPB|HUPCL|CRTSCTS|CLOCAL);
	ttyS.c_cflag |= CREAD;
	ttyS.c_lflag &= ~ICANON;
	
	ttyS.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
	ttyS.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
	ttyS.c_cc[VERASE]   = 0;     /* del */
	ttyS.c_cc[VKILL]    = 0;     /* @ */
	ttyS.c_cc[VEOF]     = 4;     /* Ctrl-d */
	ttyS.c_cc[VTIME]    = 5;     /* inter-character timer unused */
	ttyS.c_cc[VMIN]     = 0;     /* blocking read until 1 character arrives */
	ttyS.c_cc[VSWTC]    = 0;     /* '\0' */
	ttyS.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
	ttyS.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	ttyS.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	ttyS.c_cc[VEOL]     = 0;     /* '\0' */
	ttyS.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	ttyS.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	ttyS.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	ttyS.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	ttyS.c_cc[VEOL2]    = 0;     /* '\0' */

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &ttyS);
#endif
	return fd;
}

void serial_close(int fd){
	if(fd)
		close(fd);
}

int serial_read(int fd, char *buff, int rlen, int timeout){
	struct pollfd read_poll[1];
	int ret=-1, i=0;
	
	POLL_SET_IN(fd, read_poll[0]);
	/** Better wait for some time, but not too long.*/
	poll(read_poll, 1, timeout);
	if(read_poll[0].revents & POLLIN){
		pthread_mutex_lock(&serial_mutex);
		while(((ret = read(fd, buff + i, rlen))>0)&&(i < rlen))
			i+=ret;
		pthread_mutex_unlock(&serial_mutex);
		ret = i;
		buff[ret]='\0';
	}
	return ret;
}

/** TODO make it thread-safe with general 'config_t' */
int serial_write(int fd, char *buff, int wlen){
	int ret=0;
	pthread_mutex_lock(&serial_mutex);
	ret = write(fd, buff, wlen);
	pthread_mutex_unlock(&serial_mutex);
	return ret;
}
