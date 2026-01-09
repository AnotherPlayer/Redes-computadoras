#include <arpGET.h>
#include <arpSET.h>

#include <stdio.h>
#include <net/if.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>

void main(){

    int packet_socket,indice;
    packet_socket = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

    if(packet_socket == -1){

        perror("\nError al abrir el socket");
        exit(0);

    }

    else{

        

    }
    

}