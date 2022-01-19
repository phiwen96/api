/*---------- The Linux Channel ------------*/

#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netpacket/packet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

typedef unsigned char  BYTE;    /* 8-bit   */

enum _Boolean_ { FALSE=0, TRUE=1};

int create_socket(char *device)
{	int sock_fd;
   struct ifreq ifr;
   struct sockaddr_ll sll;
	memset(&ifr, 0, sizeof(ifr));
   memset(&sll, 0, sizeof(sll));

   sock_fd = socket (PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

   if(sock_fd == 0) { printf("ERR: socket creation for device: %s\n", device); return FALSE; }
   strncpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
   if(ioctl(sock_fd, SIOCGIFINDEX, &ifr) == -1) { printf(" ERR: ioctl failed for device: %s\n", device); return FALSE; }
	
	sll.sll_family      = AF_PACKET;
	sll.sll_ifindex     = ifr.ifr_ifindex;
	sll.sll_protocol    = htons(ETH_P_ALL);
	if(bind(sock_fd, (struct sockaddr *) &sll, sizeof(sll)) == -1) { printf("ERR: bind failed for device: %s\n", device); return FALSE; }
  return sock_fd;
}

void main(int argc, char ** argv)
{	int sock_fd=0; 
	BYTE stp[60] ={ 0x01, 0x80, 0xc2, 0x00, 0x00, 0x00, 0x00, 0x1c,   0x0e, 0x87, 0x85, 0x04, 0x00, 0x26, 0x42, 0x42,
					  	 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x64,   0x00, 0x1c, 0x0e, 0x87, 0x78, 0x00, 0x00, 0x00,
					  	 0x00, 0x04, 0x80, 0x64, 0x00, 0x1c, 0x0e, 0x87,   0x85, 0x00, 0x80, 0x04, 0x01, 0x00, 0x14, 0x00,
					  	 0x02, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00 };

	
	sock_fd = create_socket(argv[1]);
	if( !(sock_fd) ) { printf("no sock_fd found\n"); return; }
	write(sock_fd, stp, 60);
}