//Obtener

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

unsigned char tramaARPresp[60];

unsigned char MACorigen[6];
unsigned char IPorigen[4];
unsigned char IPdestino[4];
unsigned char etherARP[2]={0x08,0x06};
unsigned char codARPresp[2]={0x00,0x02};

int filtroARP(unsigned char *paq,int len){

    if(!memcmp(paq,MACorigen,6)){
        if(!memcmp(paq+12,etherARP,2))
            if(!memcmp(paq+20,codARPresp,2))
                if(!memcmp(paq+38,IPorigen,4))
                    return 1;
    }

    else
        return 0;

}

void recibeARPresp(int ds,unsigned char *trama){

    int tam,bandera=0;

    while(1){

        tam=recvfrom(ds,trama,1514,0,NULL,0);

        if(tam==-1)
            perror("\nError al recibir");

        else{

            bandera = filtroARP(trama,tam);

            if(bandera==1)
                break;
            
        }
    }

}

int main(){

    int packet_socket,indice;
    packet_socket=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

    if(packet_socket==-1){

        perror("\nError al abrir el socket");
        exit(0);

    }

    else{

        perror("Exito al abrir el socket");
        indice = obtenDatos(packet_socket);
        recibeARPresp(packet_socket,tramaARPresp);
        printf("\n**********La trama que se recibe de respuesta ARPes*************\n");
        imprimeTrama(tramaARPresp,60);

    }

    close(packet_socket);
    return 1;

}