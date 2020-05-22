#ifndef __FIBO_SOCKET_CLIENT_H__
#define __FIBO_SOCKET_CLIENT_H__

/*           Macro declaration              */

#define SERVER_SOCKET "/home/fibo/VScode_prj/Unix.domain"
#define MY_DEBUG printf("[%s %s] %s: %s: %d\n",__DATE__,__TIME__,__FILE__,__FUNCTION__,__LINE__);
#define MAX_INPUT_LEN 50

/*           Function declaration           */
int fibo_socket_client(void);



#endif