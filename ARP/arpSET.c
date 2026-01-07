//Enviar

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

unsigned char tramaARPsol[60]=
{0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x06,0x00,0x01,
0x08,0x00,0x06,0x04,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,'G','S','Q'};

void obtenerDatos(){



}

void obtenerIPdestino(){



}

void estructuraARPsol(unsigned char *trama){
    //Encabezado MAC
    memcpy(trama+6,MACorigen,6);
    //Mensaje de ARP
    memcpy(trama+22,MACorigen,6);
    memcpy(trama+28,IPorigen,4);
    memset(trama+32,0x00,6);
    memcpy(trama+38,IPdestino,4);
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

void imprimirTrama(){


    
}

int main(){

    int packet_socket,indice;
    packet_socket=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

    if(packet_socket == -1){

        perror("\nError al abrir el socket");
        exit(0);

    }

    else{    

        perror("Exito al abrir el socket");

        indice = obtenDatos(packet_socket);

        obtenIPdestino();
        estructuraARPsol(tramaARPsol);

        printf("\n**********La trama que se envia es*************\n");

        imprimeTrama(tramaARPsol,60);
        enviaTrama(packet_socket,indice,tramaARPsol);

    }

    close(packet_socket);
    return 1;

}