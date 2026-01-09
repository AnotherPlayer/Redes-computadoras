#include <stdio.h>
#include <net/if.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>

unsigned char tramaARPresp[1514];

unsigned char MACorigen[6];
unsigned char IPorigen[4];
unsigned char IPdestino[4];
unsigned char etherARP[2]={0x08,0x06};
unsigned char codARPresp[2]={0x00,0x02};

int obtenDatos( int ds );
int filtroARP( unsigned char *paq,int len );
void recibeARPresp( int ds,unsigned char *trama );
void imprimeTrama( unsigned char *trama, int tam );