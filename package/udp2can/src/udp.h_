/*
 * udp.h
 *
 *  Created on: 24.02.2014
 *      Author: jrenken
 */

#ifndef UDP_H_
#define UDP_H_


/* Set debug mode on/off
 *
 */
void udp_setDebug(int debug);


/* Create and bind a UDP server socket
 *
 */
int udp_openSocket(unsigned short port, int bcast);

int udp_setTarget(const char* host, unsigned short port);

int udp_sendDatagram(int fd, void* data, size_t len);

int udp_readDatagram(int fd, void* data, size_t len);

#endif /* UDP_H_ */
