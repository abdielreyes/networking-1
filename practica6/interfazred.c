//librerias
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<netinet/in.h>
#include<string.h>
//librerias basicas para programar sockets
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
//manual netdevice
#include <sys/ioctl.h>
#include <net/if.h>

unsigned char MAC[6], MASK[4], IP[4];  //Definimos las variables MAC, MASK (mascara de sub red) y la IP
void obtenerDatos(int ds) //Creamos una funcion para obtener los datos. ds = Descriptor de socket
{
    int i = 0;
    char nombre[10];
    struct ifreq nic; //Definimos una estructura ifreq del manual netdevice y la llamaremos nic
    printf("\nIntroduce la interfaz de red: "); //Pedimos la iterfaz de red
    scanf("%s", nombre);
    strcpy(nic.ifr_name, nombre);//Declaramos la funciòn strcpy para copiar la interfaz de red a nuestra estructura nic

    //Obtenemos el indice de la interfaz de red con la funciòn ioclt.
    if(ioctl(ds,SIOCGIFINDEX,&nic)==-1){ //Si devuelve -1 existio un error al obtener el indice
       perror("Error al obtener el indice"); //avisamos que existe un error 
    }
    else{ //en caso contrario se obtuvo correctamente el indice 
       printf("\nEl indice es: %d\n", nic.ifr_ifindex); //Imprimimos el valor del indice 
    }

    //Obtenemos la direccion MAC (SIOCGIFHWADDR)
    if(ioctl(ds,SIOCGIFHWADDR,&nic)==-1){ //Si devuelve -1 existio un error al obtener la direcciòn MAC
       perror("Error al obtener la MAC"); //avisamos que existe un error 
    }
    else{ //en caso contrario se obtuvo correctamente
       memcpy(MAC, nic.ifr_hwaddr.sa_data, 6); //Declaramos la funcion memcpy para copiar los 6 primeros bits en la variable MAC
       printf("\nMAC: "); //Imprimimos los valores de la direcciòn MAC
       for(i=0; i<6;i++){
           printf("%2x:",MAC[i]);//lo imprimimos de forma hexadecimal
       }
    } 

    //Obtenemos la direccion IP (SIOCGIFADDR)
    if(ioctl(ds,SIOCGIFADDR,&nic)==-1){ //Si devuelve -1 existio un error al obtener la IP
       perror("Error al obtener la IP"); //avisamos que existe un error 
    }
    else{ //en caso contrario se obtuvo correctamente
       memcpy(IP, nic.ifr_addr.sa_data, 6); //Declaramos la funcion memcpy para copiar los 4 bits en la variable IP
       printf("\nIP: "); //Imprimimos los valores de la direcciòn IP
       for(i=2; i<6;i++){
           printf("%2d:",IP[i]);//lo imprimimos de forma decimal 
       }
    }  
    //Obtenemos la mascara de subred MASK (SIOCGIFNETMASK)
    if(ioctl(ds,SIOCGIFNETMASK,&nic)==-1){ //Si devuelve -1 existio un error al obtener la MASK
       perror("Error al obtener la MASK"); //avisamos que existe un error 
    }
    else{ //en caso contrario se obtuvo correctamente
       memcpy(MASK, nic.ifr_netmask.sa_data, 6); //Declaramos la funcion memcpy para copiar los 4 bits en la variable MASK
       printf("\nMASK: "); //Imprimimos los valores de la direcciòn MASK
       for(i=2; i<6;i++){
           printf("%2d:",MASK[i]);//lo imprimimos de forma decimal 
       }
    } 
    if(ioctl(ds,SIOCGIFMTU,&nic) == -1){
	perror("Error al obtener MTU");
    }else{
	printf("\nMTU: %d",nic.ifr_mtu);
    }
}

int main(){
    int packet_socket; //Declaramos nuestro variable socket
    
    packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); /*Declaramos nuestra funciòn socket. Parametros: AF_PACKET la familia en la que 
                      trabajaremos, SOCK_RAW indica que podemos trabajar en la capa de enlace de datos y htons para colocar todos los protocolos.*/
                                     
    if(packet_socket == -1){ // Si devuelve -1 nuestro socket no se pudo abrir. 
        perror("Error al abrir el socket"); //avisamos que existe un error
        exit(1); //salimos del programa
    }
   
    else{ //En caso contrario, el socket abrio correctamente.
        perror("Exito al abrir el socket"); //avisamos que se abrio correctamente
        obtenerDatos(packet_socket); //mandamos a llamar nuestra funcion obtenerDatos, mandadole como parametro nuestro descriptor de socket  
    }
    close(packet_socket); //cerramos el socket
    printf("\n");

}
