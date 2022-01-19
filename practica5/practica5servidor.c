#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>

/*Servidor*/

int main()
{
int udp_socket,lbind,tam,lrecv;
struct sockaddr_in SERVIDOR, CLIENTE;
unsigned char buffer[100]="Hola Grupo";
unsigned char paqRec[512];
udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

if(udp_socket==-1)
{
    perror("\nError al abrir el socket");
    exit(0);
}
else
{
    perror("\nExito al abrir el socket");
    SERVIDOR.sin_family=AF_INET; /* address family: AF_INET */
    SERVIDOR.sin_port=htons(8080);   /* port in network byte order */
    SERVIDOR.sin_addr.s_addr=INADDR_ANY;
    lbind=bind(udp_socket, (struct sockaddr *)&SERVIDOR,sizeof(SERVIDOR));
    if(lbind==-1)
    {
        perror("\n ERROR EN bind");
        exit(0);
    }
    else 
    {
        perror("\n Exito en el bind");
        CLIENTE.sin_family=AF_INET; /* address family: AF_INET */
        CLIENTE.sin_port=htons(8080);   /* port in network byte order */
        CLIENTE.sin_addr.s_addr=inet_addr("172.31.248.68");
        
	lrecv=sizeof(CLIENTE);
        // Inicio de bucle recibo el mensaje 
        //
        tam=recvfrom(udp_socket,paqRec, 512, 0, (struct sockaddr *)&CLIENTE,&lrecv);
        if (tam==-1)
        {
            perror("\n Error al recibir");
            exit(0);
        }
        else
        {
            printf("\n El mensaje es :%s", paqRec);
            
            //solicito al usuario el mensaje a enviar
            //envio el mensaje. sugerencia usar fgets o gets
        }
        
    }
}
close(udp_socket);
return 0;
}
