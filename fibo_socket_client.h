#ifndef __FIBO_SOCKET_CLIENT_H__
#define __FIBO_SOCKET_CLIENT_H__
/*           Header include                 */

#include <pthread.h>

/*           Macro declaration              */

#define PACK_HEAD 0x7061636B
#define MAX_MSG_SIZE_WITH_HEAD  128 * 1024
#define MAX_RECV_LEN  4 * 1024
#define FIBO_SOCKET_PATH "/home/fibo/VScode_prj/socket.domain"
#define CLIENT_TO_SERVER_PATH "/home/fibo/VScode_prj/client_to_server.domain"
#define SERVER_TO_CLIENT_PATH "/home/fibo/VScode_prj/server_to_client.domain"
#define MY_DEBUG printf("[%s %s] %s: %s: %d\n",__DATE__,__TIME__,__FILE__,__FUNCTION__,__LINE__);
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MAX_PORT_NUM    10
#define MAX_PORT_PATH_LEN 100
#define OPEN_FAILURE_SLEEP_TIME 1
#define MAX_MSG_SIZE   16 * 1024
#define FIBO_PTHREAD_MUTEX_LOCK(mutex)                          \
        do{                                                     \
            if(pthread_mutex_lock(mutex) < 0){                  \
                printf("mutex lock failed [0x%p]\n\r",mutex);   \
            }                                                   \
            printf("mutex locked [0x%p]\n\r",mutex);                \
        }while(0);
#define FIBO_PTHREAD_MUTEX_UNLOCK(mutex)                          \
        do{                                                     \
            if(pthread_mutex_unlock(mutex) < 0){                  \
                printf("mutex unlock failed [0x%p]\n\r",mutex);   \
            }                                                   \
            printf("mutex unlocked [0x%p]\n\r",mutex);                \
        }while(0);
/*           Struct definition                  */

typedef enum{
    E_CUST_DEFAULT = 0,
    E_CUST_EE_DEFINFO,
    E_CUST_JSON_RESTORE
}e_fibo_cust_flag_type;

typedef enum{
    E_ERR_CODE_SUCCESS = 0,
    E_ERR_CODE_INPUT_ERROR,
    E_ERR_CODE_SOCKET_ERROR,
    E_ERR_CODE_CONNECT_ERROR,
}e_fibo_err_code;

typedef enum{
    FIBO_DATA_MODE = 0,
    FIBO_COMMAN_AT_MODE
}FIBO_MSG_MODE;

typedef enum{
    MSG_SYNC  = 0x0,
    MSG_ASYNC = 0x1,
    MSG_IND   = 0x2,
    MSG_MAX   = 0x3
}FIBO_MSF_TYPE;

typedef struct{
    FIBO_MSG_MODE cmd_type;
    e_fibo_cust_flag_type cust_flag;
    int error_code;
    unsigned long int usr_p;
    unsigned long int content_len;
    unsigned char data_start;
    unsigned char data_complete;
}fibo_sock_head_t;
typedef struct{
    unsigned long int txn_id;
    FIBO_MSF_TYPE msg_type;
    int msg_not_finish;
}fibo_msg_head_t;

typedef struct{
    unsigned int pack_head;
    unsigned int pack_len;
}fibo_pack_head_t;
typedef enum{
    host_modem_atport = 0, 
    modem_host_atport, 
    end_port
}fibo_thread_name;

typedef struct{
    fibo_thread_name thraed_name;
    const char from_path[MAX_PORT_PATH_LEN];
    const char to_path[MAX_PORT_PATH_LEN];
    FIBO_MSG_MODE msg_mode;
}fibo_cust_port_cfg;

typedef struct{
    int  p_from_fd;
    int  p_to_fd;
    char p_from_path[MAX_PORT_PATH_LEN];
    char p_to_path[MAX_PORT_PATH_LEN];
    fibo_thread_name p_thread_name;
    FIBO_MSG_MODE p_msg_mode;
    pthread_mutex_t p_from_mutex;
    pthread_mutex_t p_to_mutex;
}p_thread_info_t;


/*           Function declaration           */
int client_send_msg(int socketfd, char* buf, int cmd_len, char* out_buf, int* out_len, FIBO_MSG_MODE* g_port_mode);

int send_msg_to_platfrom(int socketfd, char* buf, int cmd_len, char* out_buf, int* out_len, FIBO_MSG_MODE* g_port_mode);

int send_msg_to_plat_or_modem(int socketfd, char* buf, int cmd_len, FIBO_MSG_MODE* g_port_mode,p_thread_info_t* tinfo);

int common_input_atcmd_proc(int socketfd,char* buf,int num_read,FIBO_MSG_MODE* g_port_mode, p_thread_info_t* tinfo);

void construct_signal_string(char* str, size_t size, int bits);

int init_socket_client_port(int g_socketfd);

int init_port_bridge(void);

void* host_to_modem_atport_thread(void* pthread_info);

void* modem_to_host_atport_thread(void* pthread_info);

static int assemable_packhead(char* buf, fibo_pack_head_t* st_pack);

static int assemable_msghead(char* buf, fibo_msg_head_t* st_pack);

static int assemable_sockhead(char* buf, fibo_sock_head_t* st_pack);
#endif