#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>

#define MY_DEBUG printf("[%s %s] %s: %s: %d\n",__DATE__,__TIME__,__FILE__,__FUNCTION__,__LINE__);

int main(void){
    struct sockaddr_un client_addr;
    int sockfd = socket(AF_UNIX,SOCK_STREAM,0);
    memset(&client_addr,0,sizeof (struct sockaddr_un));
    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path,"server_socket");
    if (sockfd < 0){
        perror("socket create failed:");
        return 0;
    }

    int result = connect(sockfd,(struct sockaddr *)&client_addr,sizeof(client_addr));
    if(result == -1){
        perror("connect error:");
        return 0;
    }
    char ch = 'A';
    write(sockfd,&ch,1);
    read(sockfd,&ch,1);
    printf("Get char from server: %c\n",ch);

    close(sockfd);

    return 0;
}