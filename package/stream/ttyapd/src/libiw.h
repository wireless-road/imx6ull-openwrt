#ifndef INC_LIBIW
#define INC_LIBIW


void libiw_status(void *iwptr, unsigned char **retbuf);
void libiw_scan(void *iwptr, unsigned char **retbuf);

int libiw_init(unsigned char *wlandev, void *iwptr);
void libiw_end(void **iwptr);


#endif
