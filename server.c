#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>

int main(void){
    unlink("server_socket");

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path,"server_socket");
    
    int server_sockfd = socket(AF_UNIX,SOCK_STREAM,0);
    if(server_sockfd < 0){
        perror("server socket create failed:");
        return 0;
    }

    bind(server_sockfd,(struct sockaddr *)&server_addr,sizeof(server_sockfd));

    listen(server_sockfd,5);

    char ch;
    int client_sockfd;
    struct sockaddr_un client_addr;
    socklen_t len = sizeof(client_addr);
    while(1){
        printf("server waiting:\n");
        client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_addr,&len);

        read(client_sockfd,&ch,1);
        printf("get char from client: %c\n",ch);
        ++ch;
        write(client_sockfd,&ch,1);

        close(server_sockfd);
    }
    return 0;
}