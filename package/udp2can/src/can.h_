/*
 * can.h
 *
 *  Created on: 21.02.2014
 *      Author: jrenken
 */

#ifndef CAN_H_
#define CAN_H_

#include <sys/types.h>
#include "can_driver.h"



/* special address description flags for the CAN_ID */
#define CAN_EFF_FLAG 0x80000000U /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0x40000000U /* remote transmission request */
#define CAN_ERR_FLAG 0x20000000U /* error frame */

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK 0x000007FFU /* standard frame format (SFF) */
#define CAN_EFF_MASK 0x1FFFFFFFU /* extended frame format (EFF) */
#define CAN_ERR_MASK 0x1FFFFFFFU /* omit EFF, RTR, ERR flags */

/* error class (mask) in can_id */
#define CAN__ERR_TX_TIMEOUT   0x00000001U /* TX timeout (by netdevice driver) */
#define CAN__ERR_LOSTARB      0x00000002U /* lost arbitration    / data[0]    */
#define CAN__ERR_CRTL         0x00000004U /* controller problems / data[1]    */
#define CAN__ERR_PROT         0x00000008U /* protocol violations / data[2..3] */
#define CAN__ERR_TRX          0x00000010U /* transceiver status  / data[4]    */
#define CAN__ERR_ACK          0x00000020U /* received no ACK on transmission */
#define CAN__ERR_BUSOFF       0x00000040U /* bus off */
#define CAN__ERR_BUSERROR     0x00000080U /* bus error (may flood!) */
#define CAN__ERR_RESTARTED    0x00000100U /* controller restarted */

// data[1]
#define CAN__ERR_CRTL_RX_OVERFLOW 0x01 /* RX buffer overflow */
#define CAN__ERR_CRTL_TX_OVERFLOW 0x02 /* TX buffer overflow */


#define AUTO_LED_ON		128
#define AUTO_LED_OFF		64


/*
 * Controller Area Network Identifier structure
 *
 * bit 0-28	: CAN identifier (11/29 bit)
 * bit 29	: error frame flag (0 = data frame, 1 = error frame)
 * bit 30	: remote transmission request flag (1 = rtr frame)
 * bit 31	: frame format flag (0 = standard 11 bit, 1 = extended 29 bit)
 */
typedef __uint32_t canid_t;

/*
 * Controller Area Network Error Frame Mask structure
 *
 * bit 0-28	: error class mask (see include/linux/can/error.h)
 * bit 29-31	: set to zero
 */
typedef __uint32_t can_err_mask_t;

/**
 * struct can_frame - basic CAN frame structure
 * @can_id:  the CAN ID of the frame and CAN_*_FLAG flags, see above.
 * @can_dlc: the data length field of the CAN frame
 * @data:    the CAN frame payload.
 */
struct can_frame {
	canid_t 	can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
	__uint8_t   can_dlc; /* data length code: 0 .. 8 */
	__uint8_t   data[8] __attribute__((aligned(8)));
};




/* Set debug mode on/off
 *
 */
void can_setDebug(int debug);


/* Open the CAN device
 * @return file descriptor
 */
int can_openDevice(const char* dev);


/* Set CAN interface parameter
 *
 */
int can_setParameter(int fd, unsigned char mode, unsigned int br, unsigned char br2btr);

/* Set message filter for receiving messages
 *
 */
int can_setMsgFilter(int fd, unsigned long int mask, unsigned long int code);

/* Get info about the driver
 *
 */
int can_getDriverInfo(int fd, struct DriverInfo * info);

/* Set the bus communication on or off
 *
 */
int can_setOnOff(int fd, unsigned char on);

/*
 *
 */
int can_Reset(int fd);

/*
 *
 */
int can_setFastMode(int fd, unsigned char mode);

/*
 *
 */
int can_setHighSpeed(int fd, unsigned char mode);

/*
 *
 */
int can_setTermination(int fd, unsigned char term);

/* Set debug mode (syslog)
 *
 */
int can_setDebugMode(int fd, unsigned char debug);


/* Set the debug LEDs
 *
 */
int can_setLed(int fd, unsigned char led);

/* Send a CAN message
 *
 */
int can_sendMessage(int fd, struct TCANMsg *msg);

/* Get the interface status
 *
 */
int can_getStatus(int fd, unsigned char *status);

/* Get the number of available messages in receive buffer
 * @return -1 if error or number of available messages
 */
int can_getRecMessages(int fd);

/* Read a message without timestamp
 *
 */
int can_readMessage(int fd, struct TCANMsg* msg);

/* Read a message with timestamp
 *
 */
int can_readMessageT(int fd, struct TCANMsgT* msg);

/* Dump the status to stdout
 *
 */
void can_dumpStatus(unsigned char status);

/* Dump a timeless messeage to stdout
 *
 */
void can_dumpMessage(struct TCANMsg *msg, unsigned char mode);

/* Dump ap long message to stdout
 *
 */
void can_dumpMessageT(struct TCANMsgT *msg, unsigned char mode);

#endif /* CAN_H_ */
