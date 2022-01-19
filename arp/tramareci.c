#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h> 
#include <net/ethernet.h> /* The l2 protocols */
#include <arpa/inet.h> //htons
#include <stdio.h> //perror
#include <stdlib.h> // exit
#include <unistd.h>  //close
#include <sys/ioctl.h>//netdevice
#include <net/if.h>//ioctl
#include <string.h> // exit

unsigned char MACorigen[6];
unsigned char MACbroad[6]={0xff,0xff,0xff,0xff,0xff,0xff,};
unsigned char ethertype[2]={0x0c,0x0c};
unsigned char tramaEnv[1514];
unsigned char tramaRec[1514];

int obtenerDatos (int ds)
{
    struct ifreq nic;
    unsigned char nombre[20];
    int i,index;
    printf("\033[0;36m\nInserta el nombre de la interfaz:  ");
    scanf("%s",nombre);
    strcpy(nic.ifr_name,nombre);
    if(ioctl(ds,SIOCGIFINDEX,&nic)==-1)
        {
        perror("\033[0;31m\nError al obtener el index");
        exit(0);
        }
    else
    { 
    index=nic.ifr_ifindex;
    printf("\nEl indice es: %d",index);
    if(ioctl(ds,SIOCGIFHWADDR,&nic)==-1)
    {
        perror("\033[0;31m\nError al obtener la MAC");
        exit(0);        
    }
    else
    {
        memcpy(MACorigen,nic.ifr_hwaddr.sa_data,6);
        printf("\033[0;36m\nLa MAC es:\n");
        for(i=0;i<6;i++)
        {
          printf("\033[0;37m%.2x:",MACorigen[i]);  
        }
      } 
   }
printf("\n");

   return index;
}
void estructuraTrama(unsigned char *trama)
{
    memcpy(trama+0,MACbroad,6);
    memcpy(trama+6,MACorigen,6);
    memcpy(trama+12,ethertype,2);
    memcpy(trama+14,"Mensaje en trama",30);
}

void enviarTrama(int ds, int index,unsigned char *trama)
{
    int tam;
    struct sockaddr_ll interfaz;
    memset(&interfaz,0x00,sizeof(interfaz));
    interfaz.sll_family=AF_PACKET,
    interfaz.sll_protocol=htons(ETH_P_ALL);
    interfaz.sll_ifindex=index;
    tam=sendto(ds,trama,60,0,(struct sockaddr *)&interfaz,sizeof(interfaz));
    if(tam==-1)
    {
        perror("\033[0;31m\nError al enviar:");
        exit(0);
    }
    else
        perror("\033[0;32m\n Exito al enviar");
       
}

void imprimirTrama(unsigned char *paq, int len)
{

    int i;
    for(i=0;i<len;i++)
    {
        if(i%16==0)
            printf("\n");
        printf("\033[0;33m%.2x:",paq[i]);
    }
    printf("\n");
}


void recibirTrama(int ds, unsigned char *trama)
{
    int tam;
    while(1)
    {   
    tam=recvfrom(ds,trama,1514,0,NULL,0);
    if(tam==-1)
    {
        perror("\033[0;31m\n Error al Recibir");
        exit(0);
    }
    else
    {
        imprimirTrama(trama,tam);
    }  
    }
}

int main()
{
int packet_socket,indice;
packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(packet_socket ==-1)
    {
        perror("\033[0;31m\nError al abrir el socket");
        exit(0);
    }
    else
    {
   perror("\033[0;32m\nExito al abrir el socket");
   indice=obtenerDatos(packet_socket);
   estructuraTrama(tramaEnv);
   enviarTrama(packet_socket,indice,tramaEnv);
   recibirTrama(packet_socket,tramaRec);
   
    }
    close(packet_socket);
    return 0;
}                     
    

