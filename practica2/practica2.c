#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    int udp_socket;
    udp_socket = socket(AF_INET, SOCK_STREAM,0);
    if(udp_socket == -1){
	perror("\nError al abrir el socket");
	exit(0);
    }
    else perror("\nExito al abrir el socket");
    close(udp_socket);
    return 0;

}
