#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>

#define CLIENT_TO_SERVER_PATH "/home/fibo/VScode_prj/client_to_server.domain"
#define MAX_OUTPUT_LEN 50

int main(void){
    unlink("CLIENT_TO_SERVER_PATH");

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path,CLIENT_TO_SERVER_PATH);
    
    int server_sockfd = socket(AF_UNIX,SOCK_STREAM,0);
    if(server_sockfd < 0){
        perror("server socket create failed:");
        return 0;
    }
    
    if(bind(server_sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr))<0){
        printf("bind failed\n");
        return 0;
    }

    listen(server_sockfd,5);

    char ch[MAX_OUTPUT_LEN];
    int client_sockfd;
    struct sockaddr_un client_addr;
    socklen_t len = sizeof(client_addr);
    while(1){
        printf("server waiting:\n");
        client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_addr,&len);

        read(client_sockfd,ch,MAX_OUTPUT_LEN);
        printf("get char from client: %s\n",ch);
        if(strcmp(ch,"exit") != 0){
            write(client_sockfd,ch,MAX_OUTPUT_LEN);
            close(client_sockfd);
        }
        else{
            printf("server exit\n");
            close(client_sockfd);
            break;
        }
    }
    close(server_sockfd);
    return 0;
}