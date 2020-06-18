#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stddef.h>
#include "fibo_socket_client.h"

/*        Global variable begin      */


int g_host_modem_sockfd        = -1;
FIBO_MSG_MODE g_at_port_mode   = FIBO_COMMAN_AT_MODE;

const fibo_cust_port_cfg fibo_port_cfg[] = 
    {{host_modem_atport,CLIENT_TO_SERVER_PATH,SERVER_TO_CLIENT_PATH,FIBO_COMMAN_AT_MODE},
     {modem_host_atport,SERVER_TO_CLIENT_PATH,CLIENT_TO_SERVER_PATH,FIBO_COMMAN_AT_MODE}};


/*        Global variable end         */

static int assemable_packhead(char* buf, fibo_pack_head_t* st_pack){

    if(NULL == buf || NULL == st_pack){
        printf("%s input error",__func__);
        return -1;

    }
    memcpy(buf, st_pack, sizeof(fibo_pack_head_t));
    return 0;

}

static int assemable_msghead(char* buf, fibo_msg_head_t* st_pack){

    if(NULL == buf || NULL == st_pack){
        printf("%s input error",__func__);
        return -1;

    }
    memcpy(buf, st_pack, sizeof(fibo_msg_head_t));
    return 0;

}

static int assemable_sockhead(char* buf, fibo_sock_head_t* st_pack){

    if(NULL == buf || NULL == st_pack){
        printf("%s input error",__func__);
        return -1;

    }
    memcpy(buf, st_pack, sizeof(fibo_sock_head_t));
    return 0;

}

int client_send_msg(int socketfd, char* buf, int cmd_len, char* out_buf, int* out_len, FIBO_MSG_MODE* g_port_mode){

    if(NULL == buf || NULL == out_buf || NULL == out_len || NULL == g_at_port_mode){
        printf("%s input error",__func__);
        return 0;
    }

    int iret_send = 0;
    int offset    = 0;
    char* buf_with_allhead = NULL;

    buf_with_allhead = (char*)malloc(MAX_MSG_SIZE_WITH_HEAD);
    if(NULL == buf_with_allhead){
        printf("malloc failed\n\r");
        return -1;
    }

    memset(buf_with_allhead, 0, MAX_MSG_SIZE_WITH_HEAD);

    fibo_pack_head_t st_packhead;
    fibo_msg_head_t  st_msghead;
    fibo_sock_head_t st_sockhead;

    memset(&st_packhead, 0, sizeof(fibo_pack_head_t));
    memset(&st_msghead, 0, sizeof(fibo_pack_head_t));
    memset(&st_sockhead, 0, sizeof(fibo_pack_head_t));

    //add package-head
    st_packhead.pack_head = PACK_HEAD;
    st_packhead.pack_len = sizeof(fibo_pack_head_t) + sizeof(fibo_msg_head_t) + sizeof(fibo_sock_head_t) + cmd_len;
    assemable_packhead(buf_with_allhead + offset, &st_packhead);
    offset += sizeof(st_packhead);

    //add message-head
    st_msghead.msg_not_finish = 0;
    st_msghead.msg_type = 0;
    st_msghead.txn_id = 0;
    assemable_msghead(buf_with_allhead + offset, &st_msghead);
    offset += sizeof(st_msghead);

    //add socket-head
    st_sockhead.cmd_type = cmd_len;
    st_sockhead.cust_flag = 0;
    st_sockhead.data_complete = 0;
    st_sockhead.data_start = 0;
    st_sockhead.error_code = 0;
    st_sockhead.cmd_type = *g_port_mode;
    st_sockhead.usr_p = 0;
    assemable_sockhead(buf_with_allhead + offset, &st_sockhead);
    offset += sizeof(st_sockhead);

    //add buf
    memcpy(buf_with_allhead + offset, buf, cmd_len);
    offset += cmd_len;

    buf_with_allhead[offset] = '\0';

    //send msg with all-head
    iret_send = send(socketfd, buf_with_allhead, offset, 0);

}

int send_msg_to_platfrom(int socketfd, char* buf, int cmd_len, char* out_buf, int* out_len, FIBO_MSG_MODE* g_port_mode){

    if(NULL == buf || NULL == out_buf || NULL == out_len || NULL  == g_at_port_mode){
        printf("%s inout error\n\r",__func__);
        return 0;
    }

    int iret = 0;

    //client prepare to send message
    iret = client_send_msg(socketfd, buf, cmd_len, out_buf, out_len, g_port_mode);
}

int send_msg_to_plat_or_modem(int socketfd, char* buf, int cmd_len, FIBO_MSG_MODE* g_port_mode,p_thread_info_t* tinfo){
    
    if(NULL == buf || NULL == g_port_mode || NULL == tinfo){
        printf("%s input error",__func__);
        return 0;
    }

    int iret = 0;
    char out_buf[MAX_RECV_LEN] = {"app-pla is not runnig ,plz wait....\n\r"};
    int out_len = strlen(out_buf);

    //send msg to platform
    if(*g_port_mode == FIBO_COMMAN_AT_MODE){
        iret = send_msg_to_platfrom(socketfd, buf, cmd_len, out_buf, &out_len, g_port_mode);
    }

    /*

        send to modem or something else to da

    */
}
int common_input_atcmd_proc(int socketfd, char* buf, int num_read, FIBO_MSG_MODE* g_port_mode, p_thread_info_t* tinfo){

    if(NULL == buf || NULL == g_port_mode || NULL == tinfo){
        printf("%s\n\r",__func__);
    }

    int cmd_len = 0;

    buf[num_read] = '\0';
    cmd_len = strlen(buf);
    printf("Message length = %d\n\r",cmd_len);

    int t_num_read = num_read;
    /*

          Maybe have something to do ,but now transfer msg by socket

    */

    send_msg_to_plat_or_modem(socketfd, buf, cmd_len, g_port_mode, tinfo);
    return 0;
}

void construct_signal_string(char* str, size_t size, int bits){
    //print any bits that aren't DTR/CD/RI/RIS as a mask.
    unsigned int unaccounted_bits = bits & ~(TIOCM_DTR | TIOCM_CD | TIOCM_RI | TIOCM_RTS);

    //And print DTR/CD/RI/RIS in readable format.
    snprintf(str, size -1, "0x%x%s%s%s%s",
    unaccounted_bits,
    bits & TIOCM_DTR ? " | DTR" : "",
    bits & TIOCM_CD  ? " | CD " : "",
    bits & TIOCM_RI  ? " | RI " : "",
    bits & TIOCM_RTS ? " | RTS" : "");
}

int init_socket_client_port(int g_socketfd){
    printf("FUNC ENTER: %s  init socket begin \n\r",__FUNCTION__);
    int len = 0;
    char* path = FIBO_SOCKET_PATH;
    struct sockaddr_un client_addr;
    memset(&client_addr,0,sizeof(struct sockaddr_un));

    //create client socketfd.
    if( 0 > g_socketfd){
        g_socketfd = socket(AF_UNIX,SOCK_STREAM,0);
        printf("g_socketfd = %d \n\r",g_socketfd);
    }else{
        printf("g_socketfd > 0 ,no need create socketfd,g_socketfd = %d\n\r",g_socketfd);
    }
    if( 0 > g_socketfd){
        printf("After create socketfd,g_socketfd < 0,socketfd create failed\n\r");
        return -E_ERR_CODE_SOCKET_ERROR;
    }else{
        printf("socketfd create success\n\r");
    }

    //setup for bind
    len = strlen(path);
    len = MIN(len , (int)(sizeof(client_addr.sun_path)-1));
    client_addr.sun_family = AF_UNIX;
    client_addr.sun_path[0] = '\0';
    memcpy (&client_addr.sun_path[1],path,len);

    len = 1 + offsetof(struct sockaddr_un, sun_path) + len;

    //connect to server's socketfd.
    if( 0 > connect(g_socketfd,(struct sockaddr*)&client_addr,len)){
        perror("Client connect failed\n\r");
        return -E_ERR_CODE_CONNECT_ERROR;
    }else{
        printf("Client connect success\n\r");
    }

    return 0;
}


void* modem_to_host_atport_thread(void* pthread_info){

}

void* host_to_modem_atport_thread(void* pthread_info){
    printf("%s begin....\n\r",__func__);

    int temp_fd                  = -1; 
    int ret                      = 0;
    int signals                  = 0;
    int num_read                 = 0;
    char buf[MAX_MSG_SIZE + 1];
    char signal_str_buf[MAX_MSG_SIZE + 1];
    struct pollfd pollinfo;

    if(pthread_info == NULL){
        free(pthread_info);
        pthread_info = NULL;
        return NULL;
    }
    p_thread_info_t temp_info   =  *((p_thread_info_t*)pthread_info);

    memset(&pollinfo,0,sizeof(pollinfo));
    // These are the events we care about.  They should not be changed.
    pollinfo.events = POLLIN | POLLPRI | POLLERR | POLLHUP;
    if( 0 > g_host_modem_sockfd ){
        init_socket_client_port(g_host_modem_sockfd);
    }else{
        printf("client atport has been inited\n\r");
    }

    while(1){
        if(1){

            /*PCIe related parts*/

        }

        //Open the usb file
        if (temp_info.p_from_fd == 0){
            printf("Before open USB file\n\r");
            temp_fd = open(temp_info.p_from_path,O_RDWR);
            printf("After open USB file\n\r");
            if(temp_fd < 0){
                sleep(OPEN_FAILURE_SLEEP_TIME);
                continue;
            }else{
                FIBO_PTHREAD_MUTEX_LOCK(&temp_info.p_from_mutex);
                temp_info.p_from_fd = temp_fd;
                FIBO_PTHREAD_MUTEX_UNLOCK(&temp_info.p_from_mutex);
                //Open successfully
                pollinfo.fd = temp_info.p_from_fd;
                printf("pollinfo.fd = %d\n\r",pollinfo.fd);
            }
        }
        //Use the USB file
        pollinfo.fd = 0;
        ret = poll(&pollinfo,1,-1);

        if(pollinfo.revents & POLLERR){
            printf("POLLERR is set\n\r");
        }

        if(pollinfo.revents & POLLHUP){
            printf("POLLHUP is set\n\r");
        }

        //If poll did not work as espected, or if the subsystem is not available, start over.
        if((ret < 0) || (pollinfo.revents & (POLLERR | POLLHUP))){
            printf("Sending DTR low signal to MODEM\n\r");
            signals = signals & ~TIOCM_DTR;

            construct_signal_string(signal_str_buf, MAX_MSG_SIZE, signals);

            FIBO_PTHREAD_MUTEX_LOCK(&temp_info.p_to_mutex);
            ret = ioctl(temp_info.p_to_fd,TIOCMSET,&signals);
            FIBO_PTHREAD_MUTEX_UNLOCK(&temp_info.p_to_mutex);
            
            FIBO_PTHREAD_MUTEX_LOCK(&temp_info.p_from_mutex);
            close(temp_info.p_from_fd);
            temp_info.p_from_fd = -1;
            FIBO_PTHREAD_MUTEX_UNLOCK(&temp_info.p_from_mutex);

            continue;
        }

        //POLLPRI is used to indicate a signal change.
        if(pollinfo.revents & POLLPRI){
            printf("POLLPRI is set\n\r");

            FIBO_PTHREAD_MUTEX_LOCK(&temp_info.p_from_mutex);
            ret = ioctl(temp_info.p_from_fd, TIOCMGET, &signals);
            FIBO_PTHREAD_MUTEX_UNLOCK(&temp_info.p_from_mutex);

            construct_signal_string(signal_str_buf,MAX_MSG_SIZE,signals);

            FIBO_PTHREAD_MUTEX_LOCK(&temp_info.p_to_mutex);
            ret = ioctl(temp_info.p_to_fd, TIOCMSET, &signals);
            FIBO_PTHREAD_MUTEX_UNLOCK(&temp_info.p_to_mutex);
        }
        
        //Read data if we can
        if(pollinfo.revents & POLLIN){
            printf("POLLIN is set\n\r");

            FIBO_PTHREAD_MUTEX_LOCK(&temp_info.p_from_mutex);
            num_read = read(temp_info.p_from_fd, buf, MAX_MSG_SIZE);
            FIBO_PTHREAD_MUTEX_UNLOCK(&temp_info.p_from_mutex);
            printf("read from '%s' num_read %d",temp_info.p_from_path,num_read);

            if(num_read < 0){
                FIBO_PTHREAD_MUTEX_LOCK(&temp_info.p_from_mutex);
                close(temp_info.p_from_fd);
                temp_info.p_from_fd = -1;
                FIBO_PTHREAD_MUTEX_UNLOCK(&temp_info.p_from_mutex);
                continue;
            }

            if(0 == common_input_atcmd_proc(g_host_modem_sockfd, buf, num_read,&g_at_port_mode, &temp_info)){
                
                // do something
            }
        }
    }    
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
        //printf("thread %d p_thread_name: %d\n\r",fibo_pth_num,fibo_port_thread_info[fibo_pth_num].p_thread_name );
        strcpy(&fibo_port_thread_info[fibo_pth_num].p_from_path,&fibo_port_cfg[fibo_pth_num].from_path);
        //printf("thread %d p_from_path: %s\n\r",fibo_pth_num,fibo_port_thread_info[fibo_pth_num].p_from_path);
        strcpy(&fibo_port_thread_info[fibo_pth_num].p_to_path,&fibo_port_cfg[fibo_pth_num].to_path);
        //printf("thread %d p_to_path: %s\n\r",fibo_pth_num,fibo_port_thread_info[fibo_pth_num].p_to_path);
        strcpy(&fibo_port_thread_info[fibo_pth_num].p_msg_mode,&fibo_port_cfg[fibo_pth_num].msg_mode);
        //printf("thread %d msg_mode: %d\n\r",fibo_pth_num,fibo_port_thread_info[fibo_pth_num].p_msg_mode);
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
            //printf("host to modem thread create begin\n\r");
            if(0 == pthread_create(&fibo_port_thread[fibo_pth_num],NULL,host_to_modem_atport_thread,(void*)&fibo_port_thread_info[fibo_pth_num])){
                printf("host_to_modem_atport_thread create success\n\r");
            }
            else{
                printf("host_to_modem_atport_thread create failed\n\r");
            }
        }else if((fibo_pth_num == 1 && fibo_port_thread_info[fibo_pth_num].p_thread_name == 1) == 0){
            //printf("modem to host thread create begin\n\r");
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

