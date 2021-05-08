#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <linux/can.h>

#define MAXDG   16		// maximum datagram size

unsigned char udpframe[MAXDG];

int verbose = 1;
int background = 0;
int local_port = 15731;
int destination_port = 15730;
unsigned char can_interface[4] = "can0";

int UDP_client_init(char* server_address, int server_port, struct sockaddr_in* baddr) {
    int sb;
    if ( (sb = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    baddr->sin_family = AF_INET;
    baddr->sin_port = htons(server_port);
    baddr->sin_addr.s_addr = inet_addr(server_address);
    return sb;
}

int UDP_server_init(int port_to_bind, struct sockaddr_in* saddr) {
    int sa;
    if ((sa = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr, "UDP socket error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    saddr->sin_family = AF_INET;
    saddr->sin_addr.s_addr = inet_addr("0.0.0.0"); //INADDR_ANY;
    saddr->sin_port = htons(port_to_bind);

    if (bind(sa, (struct sockaddr *)saddr, sizeof(*saddr)) < 0) {
        fprintf(stderr, "UDP bind error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sa;
}

int CAN_socker_init(char* interface) {
    int sc;
    struct ifreq ifr;
    struct sockaddr_can caddr;
    socklen_t caddrlen = sizeof(caddr);

    memset(&caddr, 0, sizeof(caddr));
    strcpy(ifr.ifr_name, interface);

    if ((sc = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        fprintf(stderr, "CAN socket error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    caddr.can_family = AF_CAN;
    if (ioctl(sc, SIOCGIFINDEX, &ifr) < 0) {
        fprintf(stderr, "CAN setup error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    caddr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sc, (struct sockaddr *)&caddr, caddrlen) < 0) {
        fprintf(stderr, "CAN bind error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sc;
}

void retransmit_udp_to_can(int udp_sd, int can_sd, char* udpframe, struct can_frame* frame) {
    int canid = 0;
    read(udp_sd, udpframe, MAXDG);
    memcpy(&canid, &udpframe[0], 4);
    frame->can_id = ntohl(canid);
    frame->can_dlc = udpframe[4];
    memcpy(&frame->data, &udpframe[5], 8);
    memcpy(&frame->data, &udpframe[5], 8);

    if (write(can_sd, frame, sizeof(*frame)) != sizeof(*frame))
        fprintf(stderr, "CAN write error: %s\n", strerror(errno));
}

void retransmit_can_to_udp(int udp_sd, char* udpframe, struct can_frame* frame, struct sockaddr_in* udp_addr) {
    int canid = 0;
    frame->can_id &= CAN_EFF_MASK;
    canid = htonl(frame->can_id);
    memcpy(udpframe, &canid, 4);
    udpframe[4] = frame->can_dlc;
    memcpy(&udpframe[5], &frame->data, frame->can_dlc);

    if (sendto(udp_sd, udpframe, 13, 0, (struct sockaddr *)udp_addr, sizeof(*udp_addr)) != 13)
        fprintf(stderr, "UDP write error: %s\n", strerror(errno));
}

void print_verbose(char* direction, struct can_frame* frame, char* udpframe) {
    if (verbose && !background) {
        fprintf(stdout,"%s CANID 0x%06X R", direction, frame->can_id);
        fprintf(stdout," [%u]", *(udpframe+4));
        for (int i = 5; i < 5 + frame->can_dlc; i++) {
            fprintf(stdout," %02x", *(udpframe+i));
        }
        fprintf(stdout,"\n");
    }
}

void print_usage(char *prg) {
    fprintf(stderr, "\nUsage: %s -l <port> -d <port> -i <can interface>\n", prg);
    fprintf(stderr, "   Version 0.93\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "         -l <port>           listening UDP port for the server - default 15731\n");
    fprintf(stderr, "         -d <port>           destination UDP port for the server - default 15730\n");
    fprintf(stderr, "         -i <can int>        can interface - default can0\n");
    fprintf(stderr, "         -f                  running in foreground\n\n");
    fprintf(stderr, "         -v                  verbose output (in foreground)\n\n");
}

void parse_args(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "l:d:b:i:hfv?")) != -1) {
        switch (opt) {
            case 'l':
                local_port = strtoul(optarg, (char **)NULL, 10);
                break;
            case 'd':
                destination_port = strtoul(optarg, (char **)NULL, 10);
//                baddr.sin_port = htons(destination_port);
                break;
            case 'i':
                strncpy(can_interface, optarg, sizeof(can_interface) - 1);
                break;
            case 'v':
                verbose = 1;
                break;
            case 'f':
                background = 0;
                break;
            case 'h':
            case '?':
                print_usage(basename(argv[0]));
                exit(EXIT_SUCCESS);
            default:
                fprintf(stderr, "Unknown option %c\n", opt);
                print_usage(basename(argv[0]));
                exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv) {
    struct can_frame frame;
    int sa, sb, sc;		// UDP socket , CAN socket, UDP Broadcast Socket
    struct sockaddr_in saddr, baddr;
    fd_set readfds;

    memset(&saddr, 0, sizeof(saddr));
    memset(&baddr, 0, sizeof(baddr));
    memset(&frame, 0, sizeof(frame));
    memset(udpframe, 0, sizeof(udpframe));

    parse_args(argc, argv);

    sb = UDP_client_init("192.168.1.65", destination_port, &baddr);
    sa = UDP_server_init(local_port, &saddr);
    sc = CAN_socker_init(can_interface);

    fprintf(stdout, "sc: %d, sb: %d, sa: %d\n", sc, sb, sa);
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sc, &readfds);
        FD_SET(sa, &readfds);
        FD_SET(sb, &readfds);

        if( select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0)
        {
            fprintf(stderr, "SELECT error: %s\n", strerror(errno));
            return -1;
        }

        // received a CAN frame
        if (FD_ISSET(sc, &readfds)) {
            if (read(sc, &frame, sizeof(struct can_frame)) < 0) {
                fprintf(stderr, "CAN read error: %s\n", strerror(errno));
            }
            else {
                retransmit_can_to_udp(sb, udpframe, &frame, &baddr);
                print_verbose("--> CAN --> UDP", &frame, udpframe);
            }
        }

        // received a UDP packet
        if (FD_ISSET(sb, &readfds)) {
            retransmit_udp_to_can(sb, sc, udpframe, &frame);
            print_verbose("--> UDP --> CAN", &frame, udpframe);
        }

        // received a UDP packet
        if (FD_ISSET(sa, &readfds)) {
            retransmit_udp_to_can(sa, sc, udpframe, &frame);
            print_verbose("--> UDP --> CAN", &frame, udpframe);
        }

    }
    close(sc);
    close(sa);
    close(sb);
    return 0;
}
