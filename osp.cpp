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

#define ETHSIZE         14
#define IPHSIZE         20
#define TCPHSIZE        20

typedef unsigned char  BYTE;             /* 8-bit   */
typedef unsigned short BYTEx2;           /* 16-bit  */
typedef unsigned long  BYTEx4;           /* 32-bit  */

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;

enum _Boolean_  { FALSE=0, TRUE=1 };

int create_socket(char *device)
{	int sock_fd;
   struct ifreq ifr;
   struct sockaddr_ll sll;
	memset(&ifr, 0, sizeof(ifr));
   memset(&sll, 0, sizeof(sll));
   
   sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
   if(sock_fd == 0) { printf("ERR: socket creation for device: %s\n", device); return FALSE; }

   strncpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
   if(ioctl(sock_fd, SIOCGIFINDEX, &ifr) == -1) { printf(" ERR: ioctl failed for device: %s\n", device); return FALSE; }

	sll.sll_family      = AF_PACKET;
	sll.sll_ifindex     = ifr.ifr_ifindex;
	sll.sll_protocol    = htons(ETH_P_ALL);
	if(bind(sock_fd, (struct sockaddr *) &sll, sizeof(sll)) == -1) { printf("ERR: bind failed for device: %s\n", device); return FALSE; }
  return sock_fd;
}

unsigned short in_cksum(u16 *ptr, int nbytes)
{
  register long sum=0;
  u16 oddbyte;
  register u16 answer;

  while(nbytes>1) { sum += *ptr++; nbytes -= 2; }
  if(nbytes == 1) { oddbyte = 0; *((unsigned char *) &oddbyte) = *(unsigned char *)ptr; sum += oddbyte; }
  sum  = (sum >> 16)+(sum & 0xffff); sum+=(sum >> 16); answer = ~sum;
  return(answer);
}

void main(int argc, char ** argv)
{
	BYTE ospf[56]={0x02,0x01,0x00,0x2c,0xc0,0xa8,0x67,0x01,0x00,0x00,0x00,0x01,0xb9,0xf8,0x00,0x01,
		0x63,0x69,0x73,0x63,0x6f,0x00,0x00,0x00,0xff,0xff,0xff,0xf8,0x00,0x0a,0x12,0x01,
		0x00,0x00,0x00,0x28,0x0a,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0xff,0xf6,0x00,0x03,
		0x00,0x01,0x00,0x04,0x00,0x00,0x00,0x01 };
	BYTE l2[14] = { 0x01, 0x00, 0x5e, 0x00, 0x00, 0x05, 0xc0, 0x01, 0x0f, 0x78, 0x00, 0x00, 0x08, 0x00};
	
	struct iphdr ip;
	memset(&ip, 0x0, IPHSIZE);
	ip.version    = 4;
   ip.ihl        = IPHSIZE >> 2;
   ip.tos        = 0;
   ip.tot_len    = htons(56+IPHSIZE);
   ip.id         = htons(rand()%65535);
   ip.frag_off   = 0;
   ip.ttl        = htons(254);
   ip.protocol   = 0x59;
   ip.saddr      = inet_addr(argv[2]);
   ip.daddr	     = inet_addr("224.0.0.5");
   ip.check      = (unsigned short)in_cksum((unsigned short *)&ip, IPHSIZE);
   
   BYTE buf[300];
   memcpy(buf, l2, ETHSIZE);
   memcpy(buf+ETHSIZE, &ip, IPHSIZE);
   memcpy(buf+ETHSIZE+IPHSIZE, ospf, 56); 
   
   int sock_fd = create_socket(argv[1]);
	if(!(sock_fd) ) { printf("no sock_fd found\n"); return; }
	write(sock_fd, (BYTE *)buf, ETHSIZE+ntohs(ip.tot_len));
}