
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TAM_msj 100

int main() {

    int udp_socket, tam, lrecv;
    struct sockaddr_in servidor, cliente;
    unsigned char msj[TAM_msj] = "Buenas red, soy Tetuan";
    unsigned char paqRec[512];
    socklen_t len_cliente;

    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (udp_socket == -1) {
        perror("Error al abrir el socket");
        exit(EXIT_FAILURE);
    }
    else
        perror("Exito al abrir el socket");
    

    memset(&servidor, 0, sizeof(servidor));
    
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(8080);     // Puerto de escucha
    servidor.sin_addr.s_addr = INADDR_ANY; // Escuchar en cualquier IP de la máquina

    if (bind(udp_socket, (const struct sockaddr *)&servidor, sizeof(servidor)) < 0) {
        perror("Error en bind");
        exit(EXIT_FAILURE);
    }
    
    else {
        perror("Exito en bind");

        lrecv = sizeof(cliente);
        tam = recvfrom(udp_socket, paqRec, 512, 0, (struct sockaddr *)&cliente, &lrecv);

        if (tam == -1) {
            perror("Error al recibir");
            exit(0);
        }
        else 
            printf("\nEl mensaje recibido es: %s\n", paqRec);
        

    }

    close(udp_socket);
    return 0;
}