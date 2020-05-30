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

/*        Global variable begin      */
int client_port_fd = -1;
int server_port_fd = -1;

int g_sockfd       = -1;
pthread_mutex_t client_port_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t server_port_mutex = PTHREAD_MUTEX_INITIALIZER;
/*        Global variable end         */
void* server_to_client_port(void* pthread_info){

}
int send_msg_process(int socketfd,char* buf,int num_read,p_thread_info_t* tinfo){
    int send_ret   = 0;
    send_ret = send(socketfd,buf,sizeof(buf),0);
    if( send_ret > 0){
        printf("send_ret : %d \n\r",send_ret);
        return send_ret;
    }
    else{
        printf("send error\n\r");
        return 0;
    }
}
int init_socket_client_port(void){
    printf("FUNC ENTER: %s  init socket begin \n\r",__FUNCTION__);
    int len = 0;
    char* path = CLIENT_TO_SERVER_PATH;
    struct sockaddr_un client_addr;
    memset(&client_addr,0,sizeof(struct sockaddr_un));
    len = strlen(path);
    len = MIN(len,(int)(sizeof(client_addr.sun_path)-1));

    client_addr.sun_family = AF_UNIX;
    client_addr.sun_path[0]='\0';
    memcpy(&client_addr.sun_path[1],path,len);
    len = 1 + offsetof(struct sockaddr_un,sun_path)+len;
    strcpy(client_addr.sun_path,CLIENT_TO_SERVER_PATH);
    g_sockfd = socket(AF_UNIX,SOCK_STREAM,0);
    if (g_sockfd < 0){
        perror("socket create failed:");
        return 1;
    }

    int result = connect(g_sockfd,(struct sockaddr *)&client_addr,len);
    if(result == -1){
        perror("connect error:");
        return 2;
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
    p_thread_info_t temp_info      = *((p_thread_info_t*)pthread_info);
    int c_num_read                 = 0;
    int ret                        = 0;
    int client_ret                 = 0; 
    int temp_fd                    = 0;
    char buffer[MAX_MSG_LEN+1];
    //struct pollfd pollinfo;

    //memset(&pollinfo,0,sizeof(struct pollfd));
    //pollinfo.events = POLLIN | POLLPRI | POLLERR | POLLHUP;

    client_ret = init_socket_client_port();
    if(client_ret == 0){
        printf("socket client init error , client_ret = %d\n\r",client_ret);
    }
    else{
        printf("socket client init success , client_ret = %d\n\r",client_ret);
    }

    while(1){
        if(*temp_info.p_from_fd < 0){
            printf("client to server bridge open begin \n\r");
            temp_fd = open(temp_info.p_from_path,O_RDWR);
            printf("client to server bridge end \n\r");
            if(temp_fd < 0){
                sleep(1);
                continue;
            }
            else{
                pthread_mutex_lock(temp_info.p_from_mutex);
                *temp_info.p_from_fd = temp_fd;
                pthread_mutex_unlock(temp_info.p_from_mutex);
                //printf("Get fd success and assign to POLLfd \n\r");
                //pollinfo.fd = *temp_info.p_from_fd;
                //printf("pollinfo.fd = %d \n\r",pollinfo.fd);
            }
                pthread_mutex_lock(temp_info.p_from_mutex);
                c_num_read = read(*temp_info.p_from_fd,buffer,MAX_MSG_LEN);
                pthread_mutex_unlock(temp_info.p_from_mutex);
                if(c_num_read < 0){
                    pthread_mutex_lock(temp_info.p_from_mutex);
                    close(*temp_info.p_from_fd);
                    pthread_mutex_unlock(temp_info.p_from_mutex);
                    continue;
                }
                if(0 == send_msg_process(g_sockfd,buffer,c_num_read,&temp_info)){
                    continue;
                }
                else{

                }
            
        }
    }
    return NULL;
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

    if(0 == pthread_join(client_port_thread,NULL)){
        printf("recive client port thread join\n\r");
    }
    if(0 == pthread_join(server_port_thread,NULL)){
        printf("recive server port thread join\n\r");
    }
}

int main(void){
    init_port_bridge();
    return 0;
}

