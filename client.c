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


void* modem_to_host_atport_thread(void* pthread_info){
    printf("%s begin....\n\r",__func__);
    p_thread_info_t temp_info        = *((p_thread_info_t*)pthread_info);
    return NULL;
}

void* host_to_modem_atport_thread(void* pthread_info){
    
}


int init_port_bridge(void){
    printf("%s begin\n\r",__func__);
    p_thread_info_t* fibo_port_thread_info;
    pthread_t* fibo_port_thread;
    int fibo_mutex_init_ret = 0;
    int fibo_pth_num = 0;
    int fibo_pth_join_num = 0;
    int fibo_thread_max_num = end_port;

    //  Memory alloc for fibo_port_thread_info
    fibo_port_thread_info = (p_thread_info_t*)malloc(fibo_thread_max_num * (sizeof(p_thread_info_t)));
    if(NULL == fibo_port_thread_info){
        printf("malloc for thread info failed\n\r");
        return 0;
    }
    bzero(fibo_port_thread_info,fibo_thread_max_num * (sizeof(p_thread_info_t)));
    printf("%p,%d\n\r",fibo_port_thread_info,fibo_thread_max_num * (sizeof(p_thread_info_t)));

    //  Memory alloc for fibo_port_thread
    fibo_port_thread = (pthread_t*)malloc(fibo_thread_max_num * (sizeof(pthread_t)));
    if(NULL == fibo_port_thread){
        printf("malloc for thread failed\n\r");
        return 0;
    }
    bzero(fibo_port_thread,fibo_thread_max_num * sizeof(pthread_t));
    printf("%p,%d\n\r",fibo_port_thread,fibo_thread_max_num * (sizeof(pthread_t)));

    //   Loop to assign thread parameters and create threads
    while(fibo_pth_num < fibo_thread_max_num){
        strcpy(&fibo_port_thread_info[fibo_pth_num].p_thread_name,&fibo_port_cfg[fibo_pth_num].thraed_name);
        printf("thread %d p_thread_name: %d\n\r",fibo_pth_num,fibo_port_thread_info[fibo_pth_num].p_thread_name );
        strcpy(&fibo_port_thread_info[fibo_pth_num].p_from_path,&fibo_port_cfg[fibo_pth_num].from_path);
        printf("thread %d p_from_path: %s\n\r",fibo_pth_num,fibo_port_thread_info[fibo_pth_num].p_from_path);
        strcpy(&fibo_port_thread_info[fibo_pth_num].p_to_path,&fibo_port_cfg[fibo_pth_num].to_path);
        printf("thread %d p_to_path: %s\n\r",fibo_pth_num,fibo_port_thread_info[fibo_pth_num].p_to_path);
        strcpy(&fibo_port_thread_info[fibo_pth_num].p_msg_mode,&fibo_port_cfg[fibo_pth_num].msg_mode);
        printf("thread %d msg_mode: %d\n\r",fibo_pth_num,fibo_port_thread_info[fibo_pth_num].p_msg_mode);
        if((fibo_mutex_init_ret = pthread_mutex_init(&fibo_port_thread_info[fibo_pth_num].p_from_mutex,NULL)) != 0){
            perror("p_from_mutex init failed\n\r");
        }
        else{
            printf("p_from_mutex init sucesss\n\r");
        }
        if((fibo_mutex_init_ret = pthread_mutex_init(&fibo_port_thread_info[fibo_pth_num].p_to_mutex,NULL)) != 0){
            perror("p_to_mutex init failed\n\r");
        }
        else{
            printf("p_to_mutex init sucesss\n\r");
        }
        if((fibo_pth_num == 0 && fibo_port_thread_info[fibo_pth_num].p_thread_name == 0) == 0){
            printf("host to modem thread create begin\n\r");
            if(0 == pthread_create(&fibo_port_thread[fibo_pth_num],NULL,host_to_modem_atport_thread,(void*)&fibo_port_thread_info[fibo_pth_num])){
                printf("host_to_modem_atport_thread create success\n\r");
            }
            else{
                printf("host_to_modem_atport_thread create failed\n\r");
            }
        }else if((fibo_pth_num == 1 && fibo_port_thread_info[fibo_pth_num].p_thread_name == 1) == 0){
            printf("modem to host thread create begin\n\r");
            if(0 == pthread_create(&fibo_port_thread[fibo_pth_num],NULL,modem_to_host_atport_thread,(void*)&fibo_port_thread_info[fibo_pth_num])){
                printf("host_to_modem_atport_thread create success\n\r");
            }
            else{
                printf("host_to_modem_atport_thread create failed\n\r");
            }
        }
        
        fibo_pth_num++;
    }


    while (fibo_pth_join_num < fibo_thread_max_num){
        pthread_mutex_destroy(&fibo_port_thread_info[fibo_pth_join_num].p_from_mutex);
        pthread_mutex_destroy(&fibo_port_thread_info[fibo_pth_join_num].p_to_mutex);
        if(0 == pthread_join(fibo_port_thread[fibo_pth_join_num],NULL)){
            printf("pthread %d join ok\n\r",fibo_pth_join_num);
        }
        fibo_pth_join_num++;
    }
    
    free(fibo_port_thread_info);
    fibo_port_thread_info = NULL;
    free(fibo_port_thread);
    fibo_port_thread = NULL;

    return 0;
}
int main(void){
    init_port_bridge();
    return 0;
}

