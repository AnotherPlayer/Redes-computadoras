//Obtener (Reply)

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

//Check --> Parece estar bien
int obtenerDatos( int ds ){

    struct ifreq red;
    char nombre[8];
    int i, index;

    // Solicita el nombre de la interfaz (ej. eth0, wlan0)
    printf("\nNombre de la interfaz de red: ");
    gets(nombre);
    strcpy(red.ifr_name, nombre);

    // 1. Obtención del índice de la interfaz
    if(ioctl(ds, SIOCGIFINDEX, &red) == -1) {
        perror("\nError al obtener el indice");
        exit(0);
    }
    index = red.ifr_ifindex;

    // 2. Obtención de la dirección MAC origen
    if(ioctl(ds, SIOCGIFHWADDR, &red) == -1) {
        perror("\nError al obtener la MAC");
        exit(0);
    }
    memcpy(MACorigen, red.ifr_hwaddr.sa_data, 6);

    // 3. Obtención de la dirección IP origen
    if(ioctl(ds, SIOCGIFADDR, &red) == -1) {
        perror("\nError al obtener la IP");
        exit(0);
    }
    // Se copian 4 bytes a partir del byte 2 por la estructura sockaddr_in
    memcpy(IPorigen, red.ifr_addr.sa_data + 2, 4);

    // Impresión de verificación de los datos locales
    printf("El indice es: %d\n", index);
    printf("La MAC origen es: ");
    for(i = 0; i < 6; i++) printf("%.2X%c", MACorigen[i], i < 5 ? ':' : ' ');
    printf("\nLa IP origen es: ");
    for(i = 0; i < 4; i++) printf("%d%c", IPorigen[i], i < 3 ? '.' : ' ');
    printf("\n");

    return index;

}

//Done --> Es del libro
int filtroARP( unsigned char *paq,int len ){

    if(!memcmp(paq,MACorigen,6)){
        if(!memcmp(paq+12,etherARP,2))
            if(!memcmp(paq+20,codARPresp,2))
                if(!memcmp(paq+38,IPorigen,4))
                    return 1;
    }

    else
        return 0;

}

//Done --> Es del libro
void recibeARPresp( int ds,unsigned char *trama ){

    int tam,bandera=0;

    while(1){

        tam = recvfrom(ds,trama,1514,0,NULL,0);

        if(tam==-1)
            perror("\nError al recibir");

        else{

            bandera = filtroARP(trama,tam);

            if(bandera==1)
                break;
            
        }
    }

}

//Check --> Parece estar bien
void imprimeTrama( unsigned char *trama, int tam ) {
    
    int i;

    for(i = 0; i < tam; i++) {

        if(i % 16 == 0) printf("\n");
            printf("%.2x ", trama[i]);

    }

    printf("\n");

}

//Done --> Es del libro
int main(){

    int packet_socket,indice;
    packet_socket = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

    if(packet_socket==-1){

        perror("\nError al abrir el socket");
        exit(0);

    }

    else{

        perror("Exito al abrir el socket");

        indice = obtenDatos(packet_socket);
        recibeARPresp(packet_socket,tramaARPresp);

        printf("\n**********La trama que se recibe de respuesta ARP's*************\n");

        imprimeTrama(tramaARPresp,60);

    }

    close(packet_socket);
    return 1;

}