//Enviar (Request)

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
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,'G','T','H'};

unsigned char MACorigen[6];
unsigned char IPorigen[4];
unsigned char IPdestino[4];
unsigned char etherARP[2]={0x08,0x06};
unsigned char codARPresp[2]={0x00,0x02};

//Check --> Parece estar bien
void obtenerDatos( int ds ){

    struct ifreq red;
    char nombre[8];
    int i, index;

    printf("\nNombre de la interfaz de red: ");
    gets(nombre); // El libro utiliza gets para capturar el nombre [1]
    strcpy(red.ifr_name, nombre);

    // 1. Obtención del índice de la interfaz [9]
    if(ioctl(ds, SIOCGIFINDEX, &red) == -1) {
        perror("\nError al obtener el indice");
        exit(0);
    }

    index = red.ifr_ifindex;

    // 2. Obtención de la dirección MAC (física) [4, 10]
    if(ioctl(ds, SIOCGIFHWADDR, &red) == -1) {
        perror("\nError al obtener la MAC");
        exit(0);
    }

    memcpy(MACorigen, red.ifr_hwaddr.sa_data, 6);

    // 3. Obtención de la dirección IP (lógica) [5, 11]
    if(ioctl(ds, SIOCGIFADDR, &red) == -1) {
        perror("\nError al obtener la IP");
        exit(0);
    }

    // Se copia a partir del byte 2 por la estructura sockaddr_in [3, 12]
    memcpy(IPorigen, red.ifr_addr.sa_data + 2, 4);

    // Impresión de datos para verificación [13, 14]
    printf("El indice es: %d\n", index);

    printf("La MAC origen es: ");
    for(i = 0; i < 6; i++)
        printf("%.2X%c", MACorigen[i], i < 5 ? ':' : ' ');

    printf("\nLa IP origen es: ");
    for(i = 0; i < 4; i++)
        printf("%d%c", IPorigen[i], i < 3 ? '.' : ' ');

    printf("\n");

    return index;

}

//Check --> Problemas con la captura de la IP destino
void obtenerIPdestino(){

    int ip[7];
    printf("\nIntroduce la IP destino (ejemplo 192.168.1.1): ");
    // El libro asume la captura de los 4 octetos [15]
    scanf("%d.%d.%d.%d", &ip, &ip[17], &ip[18], &ip[19]);
    
    IPdestino = (unsigned char)ip;
    IPdestino[17] = (unsigned char)ip[17];
    IPdestino[18] = (unsigned char)ip[18];
    IPdestino[19] = (unsigned char)ip[19];

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
void imprimirTrama( unsigned char *trama, int tam ){

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