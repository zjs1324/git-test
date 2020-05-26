#ifndef __FIBO_SOCKET_CLIENT_H__
#define __FIBO_SOCKET_CLIENT_H__
/*           Header include                 */

#include <pthread.h>

/*           Macro declaration              */

#define CLIENT_TO_SERVER_PATH "/home/fibo/VScode_prj/client_to_server.domain"
#define SERVER_TO_CLIENT_PATH "/home/fibo/VScode_prj/server_to_client.domain"
#define MY_DEBUG printf("[%s %s] %s: %s: %d\n",__DATE__,__TIME__,__FILE__,__FUNCTION__,__LINE__);
#define MAX_INPUT_LEN 50

/*           Function declaration           */

int fibo_init_socket_client(void);

int init_port_bridge(void);

void* client_to_server_port(void* pthread_info);
/*           Struct definition                  */
typedef struct{
    char* p_from_path;
    char* p_to_path;
    int*  p_from_fd;
    int*  p_to_fd;
    pthread_mutex_t* p_from_mutex;
    pthread_mutex_t* p_to_mutex;
}p_thread_info_t;

#endif