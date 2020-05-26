#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include "fibo_socket_client.h"

/*        Global variable begin      */
int client_port_fd = -1;
int server_port_fd = -1;

pthread_mutex_t client_port_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t server_port_mutex = PTHREAD_MUTEX_INITIALIZER;
/*        Global variable end         */

int fibo_init_socket_client(void){
    struct sockaddr_un client_addr;
    memset(&client_addr,0,sizeof(struct sockaddr_un));
    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path,CLIENT_TO_SERVER_PATH);
    int sockfd = socket(AF_UNIX,SOCK_STREAM,0);
    if (sockfd < 0){
        perror("socket create failed:");
        return 0;
    }

    int result = connect(sockfd,(struct sockaddr *)&client_addr,sizeof(client_addr));
    if(result == -1){
        perror("connect error:");
        return 0;
    }
#if 0 //socket test
    char ch[MAX_INPUT_LEN];
    scanf("%s",ch);
    write(sockfd,ch,MAX_INPUT_LEN);
    read(sockfd,ch,MAX_INPUT_LEN);
    printf("Get char from server: %s\n",ch);

    close(sockfd);
#endif
    return 0;
}
void* client_to_server_port(void* pthread_info){
    
}

int init_port_bridge(void){
    printf("FUNC ENTER: %s  Port bridge init begin \n\r",__FUNCTION__);
    p_thread_info_t client_port_info;
    p_thread_info_t server_port_info;

    pthread_t client_port_thread;
    pthread_t server_port_thread;

    //from client to server pthread create
    memset((void*)&client_port_info,0,sizeof(client_port_info));
    client_port_info.p_from_fd         = &client_port_fd;
    client_port_info.p_to_fd           = &server_port_fd;
    client_port_info.p_from_fd         = (char*)(CLIENT_TO_SERVER_PATH);
    client_port_info.p_to_path         = (char*)(SERVER_TO_CLIENT_PATH);
    client_port_info.p_from_mutex      = &client_port_mutex;
    client_port_info.p_to_mutex        = &server_port_mutex;

    if( 0 == pthread_create(&client_port_thread,NULL,client_to_server_port,&client_port_info)){
        printf("client to server pthread create success\n\r");
    }
    else{
        printf("client to server pthread create failed\n\r");
    }
  
    //from server to client pthread create
    memset((void*)&server_port_info,0,sizeof(server_port_info));
    server_port_info.p_from_fd         = &server_port_fd;
    server_port_info.p_to_fd           = &client_port_fd;
    server_port_info.p_from_path       = (char*)(SERVER_TO_CLIENT_PATH);
    server_port_info.p_to_path         = (char*)(CLIENT_TO_SERVER_PATH);
    server_port_info.p_from_mutex      = &server_port_mutex;
    server_port_info.p_to_mutex        = &client_port_mutex;

    if( 0 == pthread_create(&server_port_thread,NULL,server_to_client_port,&client_port_info)){
        printf("server to client pthread create success\n\r");
    }
    else{
        printf("server to client pthread create failed\n\r");
    }

}

int main(void){
    init_port_bridge();
    fibo_init_socket_client();
    return 0;
}

