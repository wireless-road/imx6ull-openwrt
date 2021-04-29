#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define OUT_AMOUNT 9


int main(int argc, char *argv[])
{
	int out[OUT_AMOUNT];
	printf("\nHello, World!\r\n");

	out[0] = open("/sys/class/gpio/gpio112/value", O_WRONLY | O_SYNC);
	out[1] = open("/sys/class/gpio/gpio107/value", O_WRONLY | O_SYNC);
	out[2] = open("/sys/class/gpio/gpio110/value", O_WRONLY | O_SYNC);
	out[3] = open("/sys/class/gpio/gpio111/value", O_WRONLY | O_SYNC);
	out[4] = open("/sys/class/gpio/gpio109/value", O_WRONLY | O_SYNC);
	out[5] = open("/sys/class/gpio/gpio108/value", O_WRONLY | O_SYNC);
	out[6] = open("/sys/class/gpio/gpio101/value", O_WRONLY | O_SYNC);
	out[7] = open("/sys/class/gpio/gpio106/value", O_WRONLY | O_SYNC);
	out[8] = open("/sys/class/gpio/gpio98/value", O_WRONLY | O_SYNC);
	while(1) {
		for(int i=0; i<OUT_AMOUNT; i++) {
			write(out[i], "0", 1);
			usleep(100000);
			write(out[i], "1", 1);
			usleep(100000);
		}
	}
}
