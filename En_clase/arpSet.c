#include <stdio.h>
#include <net/if.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>


unsigned char tramaLLC[] = "Buenas que hace, Soy Tetuán";
unsigned char MACorigen[6];
unsigned char MACdestino[6];
unsigned char longitud[2];

void estructuraTramaLLC(unsigned char *trama){

    //Encabezado MAC
    memcpy(trama+0,MACdestino,6);
    memcpy(trama+6,MACorigen,6);
    memcpy(trama+12,longitud,2);
    
    //Encabezado LLC
    trama[14]=0xf0; //DSAP
    trama[15]=0x0f; //SSAP
    trama[16]=0x7f; //Control
}

void enviaTrama(int ds,int index,unsigned char *paq){
    
    int tam;
    struct sockaddr_ll capaEnlace;

    memset(&capaEnlace, 0x00, sizeof(capaEnlace));
    capaEnlace.sll_family = AF_PACKET;
    capaEnlace.sll_protocol=htons(ETH_P_ALL);
    capaEnlace.sll_ifindex=index;
    tam = sendto(ds,paq,60,0,(struct sockaddr*)&capaEnlace,sizeof(capaEnlace));

    if(tam==-1)
        perror("\nError al enviar la trama");

    else
        perror("\nExito al enviar la trama");

}

int main(){

    unsigned char tramaARPsol[42],tramaARPresp[60];

    int packet_socket,indice;
    packet_socket=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    
    if(packet_socket==-1){
        perror("\nError al abrir el socket");
        exit(0);
    }

    else{

    perror("Exito al abrir el socket");

    indice = obtenDatos(packet_socket);

    obtenIPdestino();
    estructuraARPsol(tramaARPsol);
    enviaTrama(packet_socket,indice,tramaARPsol);
    recibeARPresp(packet_socket,tramaARPresp);
    estructuraTramaLLC(tramaLLC);

    printf("\n**********La trama que se envia es*************\n");

    imprimeTrama(tramaLLC,60);
    enviaTrama(packet_socket,indice,tramaLLC);

    }

    close(packet_socket);
    return 0;

}