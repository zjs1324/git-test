#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stddef.h>
#include "fibo_socket_client.h"

#define FIBO_SOCKET_PATH "/home/fibo/VScode_prj/socket.domain"
#define MAX_OUTPUT_LEN 50

int main(void){
    int ser_listenfd = -1;
    int len;
    char* path = FIBO_SOCKET_PATH;
    struct sockaddr_un server_addr;

    memset(&server_addr,0,sizeof(struct sockaddr_un));

    //create server listenfd.
    if( 0 > ser_listenfd){
        ser_listenfd = socket(AF_UNIX,SOCK_STREAM,0);
        printf("ser_listenfd = %d \n\r",ser_listenfd);
    }else{
        printf("ser_listenfd > 0 ,no need create socketfd,ser_listenfd = %d\n\r",ser_listenfd);
    }
    if( 0 > ser_listenfd){
        printf("After create socketfd,ser_listenfd < 0,socketfd create failed\n\r");
        return -1;
    }else{
        printf("socketfd create success\n\r");
    }

    //unlink socket path name 
    unlink(path);

    //setup for bind
    len = strlen(path);
    len = MIN(len , (int)(sizeof(server_addr.sun_path)-1));
    server_addr.sun_family = AF_UNIX;
    server_addr.sun_path[0] = '\0';
    memcpy (&server_addr.sun_path[1],path,len);

    len = 1 + offsetof(struct sockaddr_un, sun_path) + len;

    //bind socket to address
    if( 0 > bind(ser_listenfd,(struct sockaddr*)&server_addr,len)){
        perror("Server bind failed\n\r");
        return -2;
    }else{
        printf("Server bind success\n\r");
    }

    //begin listen
    if( 0 > listen(ser_listenfd,5)){
        perror("Server listen failed\n\r");
        return -3;
    }else{
        printf("Server listen success\n\r");
    }

    while(1){
        
    }
#if 1
    char ch[MAX_OUTPUT_LEN];
    int client_sockfd;
    struct sockaddr_un client_addr;
    socklen_t len = sizeof(client_addr);
    while(1){
        printf("server waiting:\n");
        client_sockfd = accept(ser_listenfd,(struct sockaddr *)&client_addr,&len);

        read(client_sockfd, ch, MAX_OUTPUT_LEN);
        printf("get char from client: %s\n",ch);
        if(strcmp(ch,"exit") != 0){
            //write(client_sockfd,ch,MAX_OUTPUT_LEN);
            close(client_sockfd);
        }
        else{
            printf("server exit\n");
            close(client_sockfd);
            break;
        }
    }
#endif
    close(ser_listenfd);
    return 0;
}