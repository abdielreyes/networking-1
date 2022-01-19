#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>

unsigned char MACorigen[6],MACdestino[6];
unsigned char MACbroad[6]={0xff,0xff,0xff, 0xff,0xff,0xff};

//solicitud de ARP
unsigned char ethertype[2]={0x08, 0x06};
unsigned char tramaEnv[1514], tramaRec[1514];
unsigned char IPorigen[4];
unsigned char IPdestino[4];
unsigned char opcodeRes[2]={0x00,0x02};


void imprimirTrama(unsigned char *paq, int len){
    int i;
    for(i=0; i<len; i++){
        if(i%16==0)
        printf("\n");
        printf("%.2x ", paq[i]);
    }
    printf("\n");
}

void estructuraTrama(unsigned char *trama){
    char IP[50];
    printf("\033[0;36m\nInserta la direccion IP:");
    gets(IP);
    inet_aton(IP,IPdestino);
    unsigned char tipoh[2]={0x00,0x01};//ethernet interfaz de red
    unsigned char tipoP[2]={0x08,0x00};//Protocolo que utiliza los servicios de ARP
    unsigned char lonH[1]={6};//longitud de las direcciones de hardware
    unsigned char longP[1]={4};//longitud de Direcciones ip
    unsigned char codO[2]={0x00,0x01};//codio de operacion
   

    //encabezado MAC
    memcpy(trama+0, MACbroad, 6);
    memcpy(trama+6, MACorigen, 6);
    memcpy(trama+12, ethertype,2);
    //Mesaje de ARP
    
    memcpy(trama+14,tipoh,2);
    memcpy(trama+16,tipoP,2);
    memcpy(trama+18,lonH,1);
    memcpy(trama+19,longP,1);
    memcpy(trama+20,codO,2);
    memcpy(trama+22, MACorigen, 6);
    memcpy(trama+28, IPorigen, 4);
    memset(trama+32, 0x00, 6);
    memcpy(trama+38,IPdestino, 4);
}
void estructuraTramaIp(unsigned char *trama, char *parmsip){
    char IP[50];
    strcpy(IP,parmsip);
    //printf("\033[0;36m\nInserta la direccion IP:");
    //gets(IP);
    inet_aton(IP,IPdestino);
    unsigned char tipoh[2]={0x00,0x01};//ethernet interfaz de red
    unsigned char tipoP[2]={0x08,0x00};//Protocolo que utiliza los servicios de ARP
    unsigned char lonH[1]={6};//longitud de las direcciones de hardware
    unsigned char longP[1]={4};//longitud de Direcciones ip
    unsigned char codO[2]={0x00,0x01};//codio de operacion
   

    //encabezado MAC
    memcpy(trama+0, MACbroad, 6);
    memcpy(trama+6, MACorigen, 6);
    memcpy(trama+12, ethertype,2);
    //Mesaje de ARP
    
    memcpy(trama+14,tipoh,2);
    memcpy(trama+16,tipoP,2);
    memcpy(trama+18,lonH,1);
    memcpy(trama+19,longP,1);
    memcpy(trama+20,codO,2);
    memcpy(trama+22, MACorigen, 6);
    memcpy(trama+28, IPorigen, 4);
    memset(trama+32, 0x00, 6);
    memcpy(trama+38,IPdestino, 4);
}


void enviarTrama( int ds, int index, unsigned char *trama){
    
    //llenar nuestra estrucutra sockaddr
    int tam;
    struct sockaddr_ll interfaz;
    memset(&interfaz, 0x00, sizeof(interfaz));
    interfaz.sll_family=AF_PACKET;
    interfaz.sll_protocol=htons(ETH_P_ALL);
    interfaz.sll_ifindex=index;
    //se hace el envio
    tam=sendto(ds, trama, 60, 0, (struct sockaddr *)&interfaz, sizeof(interfaz));

    if(tam==-1){
        perror("\033[0;31m\nError al enviar");
        exit(0);
    }
    else{
        perror("\033[0;32m\nExito al enviar");
    }
}

int filtrosARP(unsigned char *paq)
{
 unsigned char etherARP[2]={0x08,0x06};   
 if(!memcmp(paq+0,MACorigen,6)&&!memcmp(paq+12,etherARP,2)&&!memcmp(paq+20,opcodeRes,2)&&!memcmp(paq+28,IPdestino,4))
       return 1;
 else
     return 0;
}

void recibirTrama(int ds, unsigned char *trama){
    
    while(1){
        int tam;  
        tam=recvfrom(ds, trama, 60, 0, NULL, 0);
        if(tam==-1){
            perror("\033[0;31m\nError al recibir");
            exit(0);
        }
        else{
            
            if(filtrosARP(trama))
            {
                imprimirTrama(trama, tam);
                memcpy(MACdestino,trama+22,6);
                imprimirTrama(MACdestino, 6);
                break;
            }
        }
    }  
    
}
int obtenerIndice(int ds,char *interf){
    struct ifreq nic;
    unsigned char nombre[20];
    int i, index;

    //printf("\033[0;36m\nInserta el nombre de la interfaz: ");
    //gets(nombre);

    strcpy(nic.ifr_name, interf);
    if(ioctl(ds, SIOCGIFINDEX, &nic)==-1){
        perror("\033[0;31m\nError al obtener el index");
    }
    else{ 
        index=nic.ifr_ifindex;
        printf("El indice es %d", index);
        if(ioctl(ds, SIOCGIFHWADDR, &nic)==-1){
            perror("\033[0;31m\nErro al obtener la MAC");
            exit(0);
        }
        else{
            memcpy(MACorigen, nic.ifr_hwaddr.sa_data+0, 6);
            printf("\033[0;36m\nLa MAC es: ");
            for(i=0; i<6; i++){
                printf("%.2x:", MACorigen[i]);
            }
            if(ioctl(ds, SIOCGIFADDR, &nic)==-1){
            perror("\033[0;31m\nErro al obtener la IP");
            exit(0);
              }
           else{
            memcpy(IPorigen, nic.ifr_addr.sa_data+2, 4);
            printf("\033[0;36m\nLa IP es: ");
            for(i=0; i<6; i++){
                printf("%d:", IPorigen[i]);
            }
        }
           
        }
    }
    return index;
}



int obtenerDatos(int ds){
    struct ifreq nic;
    unsigned char nombre[20];
    int i, index;

    printf("\033[0;36m\nInserta el nombre de la interfaz: ");
    gets(nombre);

    strcpy(nic.ifr_name, nombre);
    if(ioctl(ds, SIOCGIFINDEX, &nic)==-1){
        perror("\033[0;31m\nError al obtener el index");
    }
    else{ 
        index=nic.ifr_ifindex;
        printf("El indice es %d", index);
        if(ioctl(ds, SIOCGIFHWADDR, &nic)==-1){
            perror("\033[0;31m\nErro al obtener la MAC");
            exit(0);
        }
        else{
            memcpy(MACorigen, nic.ifr_hwaddr.sa_data+0, 6);
            printf("\033[0;36m\nLa MAC es: ");
            for(i=0; i<6; i++){
                printf("%.2x:", MACorigen[i]);
            }
            if(ioctl(ds, SIOCGIFADDR, &nic)==-1){
            perror("\033[0;31m\nErro al obtener la IP");
            exit(0);
              }
           else{
            memcpy(IPorigen, nic.ifr_addr.sa_data+2, 4);
            printf("\033[0;36m\nLa IP es: ");
            for(i=0; i<6; i++){
                printf("%d:", IPorigen[i]);
            }
        }
           
        }
    }
    return index;
}


int main(int argc, char *argv[]){



    int packet_socket, indice;
    packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); //abrimos nuestro socket

    if(packet_socket==-1){
        perror("\033[0;31m\nError al abrir el socket");
        exit(0);
    }
    else{
        perror("\033[0;32m\nExito al abrir el socket\n");
      
	if(argc == 3){
	  char paramargv[25];
	  strcpy(paramargv,argv[1]);
	  char paramip[25];
	  strcpy(paramip,argv[2]);
	  indice=obtenerIndice(packet_socket,argv[1]);
	  estructuraTramaIp(tramaEnv,argv[2]);
	}else{
	  indice=obtenerDatos(packet_socket);//se le manda el descriptor de socket
	  estructuraTrama(tramaEnv);
	}
        enviarTrama(packet_socket, indice, tramaEnv);
        recibirTrama(packet_socket, tramaRec);
    }

    close(packet_socket); //cerramos nuestro socket

    return 0;
}



