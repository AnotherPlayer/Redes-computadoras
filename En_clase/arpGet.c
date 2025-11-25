#include <stdio.h>
#include <net/if.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>

unsigned char tramaLLC[] = "Buenas que hace, Soy Tetuán", MAC[6];

int filtroLLC(unsigned char *paq,int len){

    int tamanio;
    tamanio=(paq[12]<<8)+paq[13];

    if(tamanio<1500){
        return 1;
    }

    else
        return 0;
        
}

int recibeTramaLLC(int ds,unsigned char *trama){

    int tam,bandera=0;
    
    while(1){//Cambiar while por el de t.c

        bandera=0;
        tam=recvfrom(ds,trama,1514,0,NULL,0);

        if(tam==-1){
            
            perror("\nError al recibir");
            exit(1);

        }

        else{

            if( !( memcmp(trama+0, MAC,6) ) ){

                imprimeTrama(trama,tam);
                break;

            }

            //band/* code */era = filtroLLC(trama,tam);
            //if(bandera==1){
                //return tam;
            //}

        }
    }

}

void obtenDatos(int ds){

    struct ifreq nic;
    unsigned char nombre[20];

    printf("\nInserta el nombre de la interfaz: ");
    gets(nombre);
    strcpy(nic.ifr_name,nombre);

    if( ioctl(ds, SIOCGIFHWADDR,&nic) == -1 ){
        perror("Error al obtener el mac");
        exit(1);
    }

    else{
        memcpy(MAC, nic.ifr_hwaddr.sa_data,6);
        printf("\nLa MAC es: %02x:%02x:%02x:%02x:%02x:%02x\n",MAC[0],MAC[1],MAC[2],MAC[3],MAC[4],MAC[5]);
    }


}

void imprimeTrama(unsigned char *trama,int tam){

    int i;

    for(i=0;i<tam;i++){
        if(i%16==0)
            printf("\n");
        printf("%02x ",trama[i]);
    }
    printf("\n");

}

int main(){

    int packet_socket,indice,tam;

    packet_socket = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    
    if(packet_socket==-1){
        perror("\nError al abrir el socket");
        exit(0);
    }

    else{

        perror("Exito al abrir el socket");
        
        obtenDatos(packet_socket);
        tam = recibeTramaLLC(packet_socket,tramaLLC);
        
        printf("\n**********La trama LLC que se recibe es*************\n");

        imprimeTrama(tramaLLC,tam);
    }

    close(packet_socket);
    return 0;

}