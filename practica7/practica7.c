#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>


//variables globales

//mac origen
unsigned char MACorigen[6];
//mac dedstino
unsigned char MACbroad[6]={0xff,0xff,0xff,0xff,0xff,0xff};
//tipo de protocolo
unsigned char ethertype[2]={0x0c,0x0c};
//mensaje "trama" a enviar
unsigned char tramaEnv[1514];



//ESTRUCTURA DE LAS FUNCIONES DEL CODIGO


//FUNCION OBTENER DATOS, RECIBE UN ENTERO
int obtenerDatos (int ds){
    //estructura de datos de la interfaz
    struct ifreq nic;
    //ASIGNACION DE LA INTERFAZ
    char nombre[20];
    //VARIABLES PARA EL INDEX Y UN CONTADOR
    int i,index;
    //SOLICITAMOS LA INTERFAZ
    printf("\nInserta el nombre de la interfaz: ");
    //LEEMOS LA INTERFAZ
    scanf("%s", nombre);
    //se copia el nombre a el ifr_name de la estructura de la interdaz 
    strcpy(nic.ifr_name,nombre);
    //comprobacion sobre la interfaz, si es que esta conectada
    if(ioctl(ds,SIOCGIFINDEX,&nic)==-1){
        perror("\n Error al obtener el index");
        exit(0);
    }else{
        //se manda el indice de la estructura a la variable.
        index=nic.ifr_ifindex;
        //IMPRIMIMOS EL INDICE
        printf("\n El indice es: %d", index); 
            //se vuelve a verificar la interfaz, pero ahora con la mac
            if(ioctl(ds,SIOCGIFHWADDR,&nic)==-1){
                perror("\nError al obtener la MAC");
                exit(0);
            }else{
                //se copia en MACorigen la mac que esta en la estructura de la
		//interfaz
                memcpy(MACorigen, nic.ifr_hwaddr.sa_data,6);
                //LA IMPRIMIMOS CON UN CICLO FOR.
                printf("\n La MAC es:\n ");
                for(i=0;i<6;i++){
                    printf("%.2x:", MACorigen[i]);
                }
		printf("\n");
            }
    }
    //REGRESAMOS (RETORNAMOS) NUESTRO INDEX
    return index;
}

//ESTABLECEMOS UNA FUNCION QUE VAYA ASIGNANDO EL VALOR NECESARIO
//SIMULANDO UNA TRAMA DE RED
//La trama se envia con (macdestino, macorigen, protocolo, mensaje)
void estructuraTrama(unsigned char *trama){
    //DESTINO se enviara al broadcast, por lo que cualquier dispositivo
    //conectado a la red podra leerla
    memcpy(trama+0,MACbroad,6);
    //ORIGEN
    memcpy(trama+6,MACorigen,6);
    //TIPO DE PROTOCOLO
    memcpy(trama+12,ethertype,2);
    //DATOS A ENVIAR
    memcpy(trama+14,"hello world!!! estoy en una trama", 28);
}

//FUNCION ENVIAR TRAMA, ESTE RECIBE EL DESTINO, EL ORIGEN Y LA TRAMA A ENVIAR
void EnviarTrama (int ds, int index, unsigned char *trama){
    //tamano del la trama 
    int tam;
    //se crea una estructura de socket para la interfaz
    struct sockaddr_ll interfaz;
    //COPIAMOS LA INTERFAZ
    memset(&interfaz,0x00, sizeof(interfaz));
    //asignan las variables necesarias a la interfaz
    interfaz.sll_family=AF_PACKET;
    interfaz.sll_protocol=htons(ETH_P_ALL);
    interfaz.sll_ifindex=index;
    //se envia a trama y tam guarda el resultado para saber si se envio bien o
    //no
    tam=sendto(ds,trama,60,0,(struct sockaddr *)&interfaz, sizeof(interfaz));
    //SI ES SOLO 1, MANDA ERROR, EN OTRO CASO MANDA EL MENSAJE
    if(tam==1){
        perror("\nError al enviar");
        exit(0);
    }else perror("\nExito al enviar");
}

int main(){
    //creacion de socket
    //ASIGNAMOS UN ENTERO AL SOCKET Y EL INDICE
    int packet_socket, indice;
    //ABRIMOS NUESTRO SOCKET.
    packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    //VERIFICAMOS QUE EL SOCKET HAYA SIDO ABIERTO
    if(packet_socket == -1){
        perror("\nError al abrir el socket");
        exit(0);
    //SI ABRIMOS EL SOCKET
    }else{
        //MANDAMOS A CONSOLA UNA SEÑAL
        perror("\nExito al abrir el socket");
        //obtenemos el indice para el socket
        indice=obtenerDatos(packet_socket);
        //se inicializa la estructura de la trama 
        estructuraTrama(tramaEnv);
        //se envia la trama usando el socket que creamos
        EnviarTrama(packet_socket,indice,tramaEnv);
    }
    //CERRAMOS EL SOCKET
    close(packet_socket);
    //CONCLUIMOS EL SOCKET
    return 0;
}

