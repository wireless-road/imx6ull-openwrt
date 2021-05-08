/*
 *  (c) synertronixx GmbH
 *  Vahrenwalder Str. 7
 *  30165 Hannover
 *  Tel. : 0511 / 93 57 670
 *  Fax. : 0511 / 93 57 679
 *  Web  : www.synertronixx.de
 *  eMail: info@synertronixx.de
 * ----------------------------
 * Project       : CAN-Driver for CAN2Web SCB9328
 * Name          : can_driver.h
 * Version       :
 * Date          : 07.07.2010
 * Author        : Blaschke
 */

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

/*#include <linux/time.h> */

#include <linux/ioctl.h>

/* error messages */
#define CAN_ERR_OK                      0x00 /* no error */
#define CAN_ERR_XMTFULL                 0x01 /* CAN-Controller receive buffer full */
#define CAN_ERR_OVERRUN                 0x02 /* CAN-Controller overun */
#define CAN_ERR_BUSERROR                0x04 /* Bus error: Error counter on limit */
#define CAN_ERR_BUSOFF                  0x08 /* Bus error: CAN-Controller in state 'Bus-Off' */
#define CAN_ERR_RECEIVEBUF_OVERFLOW     0x10 /* SW-Receive-buffer overflow, CAN-msg. can be lost overwrite the oldest msg. */
#define CAN_ERR_TRANSMITBUF_OVERFLOW    0x20 /* SW-Transmit-Buffer overflow, CAN-msg. was not written to transmit buffer */

/* Error messages for initialisation */
#define CAN_INIT_OK                     0x00 /* init was sucessfully */
#define CAN_INIT_ERR_PARAMETER          0x01 /* wrong parameter for init */
#define CAN_INIT_ERR_HARDWARE           0x02 /* SJA1000 not available, hw err */

/* size of CAN-IDs: use 11 or 29-Bit CAN-msgs. or both 11/29-Bit mixed CAN-msgs. */
enum TCANMode {
	CAN_STANDARD,  // CAN2.0A
	CAN_EXTENDED,  // CAN2.0B
    CAN_MIXED      // CAN2.0A/CAN2.0B mixed mode
};

#define IOCTL_TYPE 'a'

/* ioctl control commands */
#define CAN_SET_SETTINGS     _IO(IOCTL_TYPE , 1)
#define CAN_GET_SETTINGS     _IO(IOCTL_TYPE , 2)
#define CAN_GET_DRIVER_INFO  _IO(IOCTL_TYPE , 3)

#define CAN_SET_ON_OFF       _IO(IOCTL_TYPE , 20)
#define CAN_SET_120OHM       _IO(IOCTL_TYPE , 21)
#define CAN_SET_FASTMODE     _IO(IOCTL_TYPE , 22)
#define CAN_SET_AUTO_LED     _IO(IOCTL_TYPE , 23)
#define CAN_SET_LED          _IO(IOCTL_TYPE , 24)
#define CAN_SET_HIGHSPEED    _IO(IOCTL_TYPE , 25)
#define CAN_SET_DEBUGMODE    _IO(IOCTL_TYPE , 26)

#define CAN_GET_STATE        _IO(IOCTL_TYPE , 30)
#define CAN_GET_RECEIVE_MSG  _IO(IOCTL_TYPE , 31)
#define CAN_GET_TRANSMIT_MSG _IO(IOCTL_TYPE , 32)

/* struct for CAN messages */
struct TCANMsg {
	unsigned long int ID;           /* 11/29 bit identifier */
	unsigned char     RTR;          /* TRUE if REMOTE frame */
	unsigned char     LEN;          /* Mode Info:
                                      Bit 6: =0 CAN2.0A or CAN2.0B mode (see mode info in CANSettings struct)
                                             =1 CAN2.0A/CAN2.0B mixed mode
                                      Bit 5: if Bit 6 = 1
                                             =0 CAN2.0A message
                                             =1 CAN2.0B message
                                      Bit 3..0: Number of valid data bytes bit 3..0 (value=0...8) */
	unsigned char     DATA[8];      /* Databytes */
};

/* struct for CAN messages with receive time */
struct TCANMsgT {
	unsigned long int ID;           /* 11/29 bit identifier */
	unsigned char     RTR;          /* TRUE if REMOTE frame */
	unsigned char     LEN;          /* Mode Info:
                                      Bit 6: =0 CAN2.0A or CAN2.0B mode (see mode info in CANSettings struct)
                                             =1 CAN2.0A/CAN2.0B mixed mode
                                      Bit 5: if Bit 6 = 1
                                             =0 CAN2.0A message
                                             =1 CAN2.0B message
                                      Bit 3..0: Number of valid data bytes bit 3..0 (value=0...8) */
	unsigned char     DATA[8];      /* Databytes */
	long              TIMES;        /* receive time seconds */
	long              TIMEUS;       /* receive time micro seconds */
};

/* struct for CAN settings */
struct CANSettings {
	unsigned char mode;             /* 0=11Bit, 1=29-Bit, 2=11/29Bit mixed mode CAN messages */
	unsigned int  baudrate;         /* 5, 10, 20, 50, 100, 125, 250, 500, 1000 */
	unsigned char baudrate_to_btr;  /* 1: set baudrate value to BTR register else baudrate is value in KBits/s */
	unsigned long int mask;         /* CAN-Mask-Reg 11/29-Bit msg filter */
	unsigned long int code;         /* CAN-Code-Reg 11/29-Bit msg filter */
};

/* struct containing further information about CAN bus driver */
#define DRIVER_INFO_STR_SIZE 128
struct DriverInfo {
	char     version[DRIVER_INFO_STR_SIZE]; /* name, version, date of CAN driver */
	unsigned long int total_rcv;            /* total CAN-msgs received since last driver init. */
	unsigned long int total_snd;            /* total CAN-msgs received since last driver init.  */
	unsigned int      rcv_buffer_size;      /* size of CAN-msg. receive buffer */
	unsigned int      snd_buffer_size;      /* size of CAN-msg. send/transmit buffer  */
};

#endif
