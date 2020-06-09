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


int g_sockfd       = -1;

const fibo_cust_port_cfg fibo_port_cfg[] = 
    {{host_modem_atport,CLIENT_TO_SERVER_PATH,SERVER_TO_CLIENT_PATH,FIBO_COMMAN_AT_MODE},
     {modem_host_atport,SERVER_TO_CLIENT_PATH,CLIENT_TO_SERVER_PATH,FIBO_COMMAN_AT_MODE}};


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
#if 0
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
    #endif
    return NULL;
}

int init_port_bridge(void){

    p_thread_info_t* fibo_port_thread;
    int fibo_temp_pth_num = 0;
    int fibo_pth_num = 0;
    int fibo_thread_max_num = end_port;
    fibo_thread_name fibo_port_thread_init = host_modem_atport;

    fibo_port_thread = (p_thread_info_t*)malloc(fibo_thread_max_num * (sizeof(p_thread_info_t)));
    if(NULL == fibo_port_thread){
        printf("malloc for thread failed\n\r");
        return 0;
    }
    bzero(fibo_port_thread,fibo_thread_max_num * (sizeof(p_thread_info_t)));
    printf("%p,%d\n\r",fibo_port_thread,fibo_thread_max_num * (sizeof(p_thread_info_t)));
    return 0;

#if 0
    while(fibo_port_thread_init != 0){
        printf("fibo_port_thread %d init begin in %s\n\r",fibo_port_thread_init,__func__);
        fibo_port_thread = (p_thread_info_t*)malloc(sizeof(p_thread_info_t)); 
    }
}
#endif
}
int main(void){
    init_port_bridge();
    return 0;
}

