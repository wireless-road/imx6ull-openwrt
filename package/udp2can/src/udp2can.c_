/*
 *  file:		udp2can.c
 *  author:		jrenken
 *  date:		24.02.2014
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

#include "can.h"
#include "can_driver.h"


#define U2C_DEBUG(format, ...)		if (debug) { fprintf(stderr, format, ##__VA_ARGS__); }

int debug = 0;

char canDevice[32] = "/dev/can0";
char udpTarget[64] = "";
unsigned short udpTargetPort = 31333;
unsigned short udpPort = 30333;

int canBitrate = 125;
int canMode = CAN_STANDARD;
int canBTR = 0;
int canHighSpeed = 0;
int canFastMode = 0;
int canTermination = 0;
unsigned long int	canFilterMask = 0;
unsigned long int canFilterCode = 0;
int canDebugMode = 0;

int canFd = -1;
int udpFd = -1;


void signalHandler(sig) /* signal handler function */
{
	switch(sig) {
		case SIGHUP:
			/* rehash the server */
			break;
		case SIGTERM:
			/* finalize the server */
			exit(0);
			break;
	}
}

void setConfigValue(const char *key, const char* value)
{
	char *endPtr;
	if (!strcmp(key, "canDevice")) {
		strncpy(canDevice, value, sizeof(canDevice));
		U2C_DEBUG("Set canDevice: %s\n", canDevice);
	} else if (!strcmp(key, "canBitrate")) {
		int br = strtol(value, &endPtr, 10);
		if (value != endPtr) {
			canBitrate = br;
			U2C_DEBUG("Set canBitrate: %d\n", canBitrate);
		}
	} else if (!strcmp(key, "canMode")) {
		canMode = atoi(value);
		U2C_DEBUG("Set canMode: %d\n", canMode);
	} else if (!strcmp(key, "canBTR")) {
		int btr = strtol(value, &endPtr, 16);
		if (value != endPtr) {
			canBTR = btr;
			U2C_DEBUG("Set canBTR: %04x\n", canBTR);
		}
	} else if (!strcmp(key, "canFastMode")) {
		canFastMode = atoi(value);
		U2C_DEBUG("Set canFastMode: %d\n", canFastMode);
	} else if (!strcmp(key, "canHighSpeed")) {
		canHighSpeed = atoi(value);
		U2C_DEBUG("Set canHighSpeed: %d\n", canHighSpeed);
	} else if (!strcmp(key, "canTermination")) {
		canTermination = atoi(value);
		U2C_DEBUG("Set canTermination: %d\n", canTermination);
	} else if (!strcmp(key, "canFilterMask")) {
		unsigned long int flt = strtol(value, &endPtr, 16);
		if (value != endPtr) {
			canFilterMask = flt;
			U2C_DEBUG("Set canFilterCode: %08lx\n", canFilterMask);
		}
	} else if (!strcmp(key, "canFilterCode")) {
		unsigned long int flt = strtol(value, &endPtr, 16);
		if (value != endPtr) {
			canFilterCode = flt;
			U2C_DEBUG("Set canFilterCode: %08lx\n", canFilterCode);
		}
	} else if (!strcmp(key, "canDebugMode")) {
		canDebugMode = atoi(value);
		U2C_DEBUG("Set canDebugMode: %d\n", canDebugMode);

	} else if (!strcmp(key, "udpPort")) {
		unsigned short prt = strtol(value, &endPtr, 10);
		if (value != endPtr) {
			udpPort = prt;
			U2C_DEBUG("Set udpPort: %d\n", udpPort);
		}

	} else if (!strcmp(key, "udpTarget")) {
		strncpy(udpTarget, value, sizeof(udpTarget));
		U2C_DEBUG("Set udpTarget: %s\n", udpTarget);
	} else if (!strcmp(key, "udpTargetPort")) {
		unsigned short prt = strtol(value, &endPtr, 10);
		if (value != endPtr) {
			udpTargetPort = prt;
			U2C_DEBUG("Set udpTargetPort: %d\n", udpTargetPort);
		}

	}
}

void readConfigFile()
{
    char 	line[128];
    char	*ptr, *savePtr;
    char	*key, *value;

	FILE *cfgFile = fopen("/etc/udp2can.conf", "r");
    if (cfgFile == NULL) {
    	fprintf(stderr, "can't open config file: %d, %s\n", errno, strerror(errno));
    	exit(EXIT_FAILURE);
    }

	while ( fgets( line, sizeof line, cfgFile ) != NULL ) {
		ptr = line;
		while (isspace(*ptr) ) ptr++;
		if ((*ptr == '#') || (*ptr == 0))
			continue;

		key = strtok_r(ptr, "\t ", &savePtr);
		if (key) {
			value = strtok_r(NULL, "\t\n ", &savePtr);
			if (value) {
				setConfigValue(key, value);
			}
		}
	}

    fclose(cfgFile);
}

void printUsage()
{
	printf("usage: udp2can [-d] [-v] [-h]\n");
	printf("       Start a UDP server and redirect incoming CAN-messages to the can bus\n");
	printf("       For UDP and CAN setting see /etc/udp2can.conf\n");
	printf("    -v:     print version information\n");
	printf("    -d:     don't daemonize and print debug messages\n");
	printf("    -h:     print this\n");

}


int initCanDevice()
{
	int result = 0;

	canFd = can_openDevice("/dev/can0");
	if (canFd < 0)
		return -1;

	if (!debug) {
		can_setDebugMode(canFd, canDebugMode);
	}

	if (canBTR == 0) {
		if (can_setParameter(canFd, canMode, canBitrate, 0) < 0)
			return -1;
	} else {
		if (can_setParameter(canFd, canMode, canBTR, 1) < 0)
			return -1;
	}

	if (canFilterMask != 0) {
		can_setMsgFilter(canFd, canFilterMask, canFilterCode);
	}

//	can_setFastMode(canFd, canFastMode);
	can_setHighSpeed(canFd, canHighSpeed);
	can_setTermination(canFd, canTermination);

	if (debug) {
		struct DriverInfo info;
		if (can_getDriverInfo(canFd, &info) == 1)  {
		    printf("\nDriver info:\n");
		    printf("Name: '%s'\n",  info.version);
		    printf("Received since last parameter setting :%lu\n",   info.total_rcv);
		    printf("Send since last parameter setting     :%lu\n",   info.total_snd);
		    printf("CAN-msg. Receive buffer size          :%lu\n",   info.rcv_buffer_size);
		    printf("CAN-msg. Send buffer size             :%lu\n\n", info.snd_buffer_size);
		}
	}

	can_setLed(canFd, AUTO_LED_ON);
	if (can_setOnOff(canFd, 1) < 0)
		return -1;

	return 0;
}

int initUDPSocket()
{
	udpFd = udp_openSocket(udpPort);
	if (udpFd < 0)
		return udpFd;

	if (udpTargetPort > 0) {
		if (udp_setTarget(udpTarget, udpTargetPort) < 0)
			return -1;
	}

	return 0;
}

void createErrorFrame(struct can_frame * frame, unsigned char status)
{
	int i;
	frame->can_id = CAN_ERR_FLAG;
	frame->can_dlc = 8;
	for (i = 0; i < 8; i++) frame->data[i] = 0;
	if (status & CAN_ERR_OVERRUN) {
		frame->can_id |= CAN__ERR_CRTL;
		frame->data[1] |= CAN__ERR_CRTL_RX_OVERFLOW;
	}
	if (status & CAN_ERR_BUSERROR)             frame->can_id |= CAN__ERR_BUSERROR;
	if (status & CAN_ERR_BUSOFF)               frame->can_id |= CAN__ERR_BUSOFF;
	if (status & CAN_ERR_RECEIVEBUF_OVERFLOW) {
		frame->can_id |= CAN__ERR_CRTL;
		frame->data[1] |= CAN__ERR_CRTL_RX_OVERFLOW;
	}
	if (status & CAN_ERR_TRANSMITBUF_OVERFLOW) {
		frame->can_id |= CAN__ERR_CRTL;
		frame->data[1] |= CAN__ERR_CRTL_TX_OVERFLOW;
	}

}

void msg2frame(struct TCANMsg * msg, struct can_frame * frame)
{
	int i;
	frame->can_id = msg->ID;
	if (msg->RTR)
		frame->can_id |= CAN_RTR_FLAG;
	if (canMode == CAN_EXTENDED ) {
		frame->can_id |= CAN_EFF_FLAG;
	} else if ((msg->LEN & 0x60) == 0x60) {
		frame->can_id |= CAN_EFF_FLAG;
	}
	frame->can_dlc = msg->LEN & 0x0F;
	for (i = 0; i < 8; i++)
		frame->data[i] = msg->DATA[i];
}

void frame2msg(struct can_frame * frame, struct TCANMsg * msg)
{
	int i;
	msg->ID = frame->can_id & 0x1FFFFFF;
	msg->LEN = frame->can_dlc;
	if (canMode == CAN_MIXED) {
		msg->LEN |= 0x20;
		if (frame->can_id & CAN_EFF_FLAG)
			msg->LEN |= 0x40;
	}
	if (frame->can_id & CAN_RTR_FLAG)
		msg->RTR = 1;
	else
		msg->RTR = 0;
	for (i = 0; i < 8; i++)
		msg->DATA[i] = frame->data[i];
}

void run()
{
	fd_set 	readfds;
	int 	maxfd = (canFd > udpFd) ? canFd + 1 : udpFd + 1;

	struct TCANMsg		msg;
	struct	can_frame	frame;
	unsigned char		status;
	static unsigned char 	prevStatus = 0;

	while (1) {

		FD_ZERO(&readfds);
		FD_SET(canFd, &readfds);
		FD_SET(udpFd, &readfds);

		usleep(5);
		int ret = select(maxfd, &readfds, NULL, NULL, NULL);
		if (ret > 0) {
			/* received a CAN frame */
			if (FD_ISSET(canFd, &readfds)) {
				int n;
				while ((n = can_getRecMessages(canFd)) > 0) {
					if (can_readMessage(canFd, &msg) == sizeof(struct TCANMsg)) {
						msg2frame(&msg, &frame);
						udp_sendDatagram(udpFd, &frame, sizeof(struct can_frame));
					}
				}
				if (can_getStatus(canFd, &status) >= 0) {
					status &= 0xFE;
					if (status != prevStatus) {
						U2C_DEBUG("Get Status: %d, %d\n", status, prevStatus);
						prevStatus = status;
						if (status != CAN_ERR_OK) {
							createErrorFrame(&frame, status);
							udp_sendDatagram(udpFd, &frame, sizeof(struct can_frame));
						}
					}
				}
			}
			if (FD_ISSET(udpFd, &readfds)) {
				if (udp_readDatagram(udpFd, &frame, sizeof(frame)) == sizeof(struct can_frame)) {
					if ((frame.can_id & CAN_ERR_FLAG) && (frame.can_id & CAN__ERR_RESTARTED)) {
						can_Reset(canFd);
						U2C_DEBUG("Restarted CAN bus\n");
					} else {
						frame2msg(&frame, &msg);
						can_sendMessage(canFd, &msg);
					}
				}
			}
		}
	}
}

int main(int argc, char** argv)
{
	int opt;
	int pid;

	while ((opt = getopt(argc, argv, "vdh")) != -1) {
		switch(opt)  {
		case 'v':
			printf("udp2can V.02 " __DATE__" " __TIME__ "\n(C) Marum, 2014\n   renken@marum.de\n");
			exit(EXIT_SUCCESS);
		case 'h':
			printUsage();
			exit(EXIT_SUCCESS);
		case 'd':
			debug = 1;
			printf("Start in foreground, debug messages are printed\n");
			break;
		default:
			printf("Unknown option!\n");
			printUsage();
			exit(EXIT_FAILURE);
		}
	}

	signal(SIGTERM, signalHandler);

	if (debug) {
		can_setDebug(1);
		udp_setDebug(1);
	}

	readConfigFile();


	if ((initCanDevice() < 0) || (initUDPSocket() < 0)) {
		fprintf(stderr, "Unable to initialze CAN device or UDP socket\nAbort.....\n");
		close(canFd);
		close(udpFd);
		exit(EXIT_FAILURE);
	}


	if (debug == 0) {
		/* Fork off the parent process */
		pid = fork();
		if (pid < 0) {
			exit(EXIT_FAILURE);
		}

		if (pid > 0) {
			printf("Going into background ...\n");
			exit(EXIT_SUCCESS);
		}
	}

	run();

	return EXIT_SUCCESS;
}
