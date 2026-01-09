//Enviar (Request)

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

unsigned char tramaARPsol[60]=
{0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x06,0x00,0x01,
0x08,0x00,0x06,0x04,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,'G','T','H'};

unsigned char MACorigen[6];
unsigned char IPorigen[4];
unsigned char IPdestino[4];
unsigned char etherARP[2]={0x08,0x06};
unsigned char codARPresp[2]={0x00,0x02};

int obtenDatos( int ds );
void obtenIPdestino();
void estructuraARPsol( unsigned char *trama );
void enviaTrama( int ds,int index,unsigned char *paq );
void imprimeTrama( unsigned char *trama, int tam );

//Check --> Parece estar bien
int obtenDatos( int ds ){

    struct ifreq red;
    char nombre[20];
    int indice;

    // 1. Solicitar el nombre de la interfaz al usuario
    printf("\nIngresa el nombre de la interfaz de red (ej. eth0, wlan0): ");
    scanf("%s", nombre);

    //Obtener índice
    strcpy(red.ifr_name, nombre);
    
    if (ioctl(ds, SIOCGIFINDEX, &red) == -1) {
        perror("\nError al obtener el indice de la interfaz");
        exit(0);
    }

    indice = red.ifr_ifindex;
    printf("\n-> Indice obtenido: %d", indice);

    //Obtener MAC
    strcpy(red.ifr_name, nombre);
    
    if (ioctl(ds, SIOCGIFHWADDR, &red) == -1) {
        perror("\nError al obtener la direccion MAC");
        exit(0);
    }
    
    memcpy(MACorigen, red.ifr_hwaddr.sa_data, 6);
    
    printf("\n-> MAC obtenida: %02X:%02X:%02X:%02X:%02X:%02X", 
           MACorigen[0], MACorigen[1], MACorigen[2], 
           MACorigen[3], MACorigen[4], MACorigen[5]);

    //Obtener la IP
    strcpy(red.ifr_name, nombre);
    
    if (ioctl(ds, SIOCGIFADDR, &red) == -1) {
        perror("\nError al obtener la direccion IP");
        exit(0);
    }
    
    memcpy(IPorigen, red.ifr_addr.sa_data + 2, 4);
    
    printf("\n-> IP obtenida: %d.%d.%d.%d\n", 
           IPorigen[0], IPorigen[1], IPorigen[2], IPorigen[3]);

    return indice;

}

//Check --> Parece estar bien
void obtenIPdestino(){

    char ipStr[20];
    printf("\nIntroduce la IP destino (a quien buscas): ");
    scanf("%s", ipStr);
    
    // Convertir string "192.168.1.1" a bytes
    in_addr_t ip = inet_addr(ipStr);
    memcpy(IPdestino, &ip, 4);

}

//Done --> Es del libro
void estructuraARPsol( unsigned char *trama ){

    //Encabezado MAC
    memcpy(trama+6,MACorigen,6);
    //Mensaje de ARP
    memcpy(trama+22,MACorigen,6);
    memcpy(trama+28,IPorigen,4);
    memset(trama+32,0x00,6);
    memcpy(trama+38,IPdestino,4);

}

//Done --> Es del libro
void enviaTrama( int ds,int index,unsigned char *paq ){

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

//Check --> Parece estar bien
void imprimeTrama( unsigned char *trama, int tam ){

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