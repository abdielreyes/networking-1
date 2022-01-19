#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

int main()
{
int udp_socket,lbind,tam;
struct sockaddr_in local, remota;
unsigned char buffer[128]="Mensaje en trama"; //el bufer lo cambio a 512
udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

if(udp_socket==-1)
{
    perror("\nError al abrir el socket");
    exit(0);
}
else
{
    perror("\nExito al abrir el socket");
    local.sin_family=AF_INET; /* address family: AF_INET */
    local.sin_port=htons(0);   /* port in network byte order */
    local.sin_addr.s_addr=INADDR_ANY;
    lbind=bind(udp_socket, (struct sockaddr *)&local,sizeof(local));
    if(lbind==-1)
    {
        perror("\n ERROR EN bind");
        exit(0);
    }
    else 
    {
        perror("\n Exito en el bind");
        remota.sin_family=AF_INET; /* address family: AF_INET */
        remota.sin_port=htons(8080);   /* port in network byte order, cambio puerto serv */
        //remota.sin_addr.s_addr=inet_addr("8.8.8.8");
        remota.sin_addr.s_addr=inet_addr("172.31.248.68");
    //iniciar ciclo while o dowhile
    // capturar el mensaje con un fgets o con gets entonces envio mensaje
        tam=sendto(udp_socket, buffer,strlen(buffer)+1, 0, (struct sockaddr *)&remota, sizeof(remota));
        if (tam==-1)
        {
            perror("\n Error al enviar");
            exit(0);
        }
        else
        {
            perror("\n Exito al enviar");
            
            // me pongo a recibir el mensaje de respuesta 
            // imprimo el mensaje
        }
        
    }
}
close(udp_socket);
return 0;
}
