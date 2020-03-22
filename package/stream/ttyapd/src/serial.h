
#ifndef INC_SERIAL
#define INC_SERIAL


int serial_read(int fd, char *buff, int rlen, int timeout);
int serial_write(int fd, char *buff, int wlen);
int serial_opentty(char *ttyname);
void serial_close(int fd);


#endif
