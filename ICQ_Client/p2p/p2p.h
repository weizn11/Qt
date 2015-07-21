#ifndef P2P_H_INCLUDED
#define P2P_H_INCLUDED

#include "../global.h"
#include "../basic/thread.h"

#define P2P_SERVER_IP SERVER_IP
#define P2P_SERVER_PORT 7502
#define UDP_BUFFER_MAXIMUM_SIZE 65000
#define P2P_DATA_MAXIMUM_SIZE 2048
#define P2P_RECV_TIMEOUT 1
#define P2P_RESEND_OF_TIMES 60
#define P2P_PROTO_DATA_MAX_SIZE 7168
#define P2P_ACKNOWLEDGMENT_NUM "8083e31dea396835f489227f262fa2b0"

typedef struct
{
    unsigned long int acknowledgementNum;    //确认号
    //unsigned int protocol;           //上层协议类型  0:校验包   1:CONN_INFO
} P2P_HEADER;

typedef struct
{
    int flag;
    struct sockaddr_in toAddr;
    char toID[ID_MAXIMUM_SIZE];
    char fromID[ID_MAXIMUM_SIZE];
} P2P_CONN_INFO;

typedef struct
{
    P2P_CONN_INFO P2PConnInfo;
    class P2P *pP2PClass;
}P2P_CREATE_CONN_THREAD_PARA;

typedef struct
{
    unsigned int initFlag;
    unsigned long int preSendAcknowledgemwntNum;
    unsigned long int preRecvAcknowledgemwntNum;
} P2P_TRAN_INFO_DATA;

typedef struct _P2P_CONNECTION_LIST_
{
    int effFlag;
    char toID[ID_MAXIMUM_SIZE];
    SOCKET soc;
    struct sockaddr_in addr;
    struct _P2P_CONNECTION_LIST_ *next;
} P2P_CONN_LIST;

typedef struct
{
    unsigned int sequenceNum;   //序列号
    int continueFlag;      //1:continue    0:finish
    unsigned long dataSize;
    char fileData[5000];
}P2P_TRAN_FILE;

typedef struct
{
    //1:校验号  2:文件流
    unsigned int proto;    //上层协议类型
    char data[P2P_PROTO_DATA_MAX_SIZE];     //协议数据
} P2P_PACKET;

class P2PCreateConnThread:public Thread
{
public:
    ~P2PCreateConnThread();
protected:
    int func(void *threadPara);
};

class P2P
{
public:
    P2P();
    ~P2P();
    friend class P2PCreateConnThread;
    int p2p_create_connection_thread(P2P_CREATE_CONN_THREAD_PARA *threadPara);
    int p2p_client_list_search(char *toID,P2P_CONN_LIST **pP2PConnClient);
    int p2p_client_list_del_node(char *toID);
    int p2p_client_list_add_addr(char *toID,struct sockaddr_in addr);
    int p2p_send(SOCKET soc,P2P_TRAN_INFO_DATA *tranData,char *buffer,unsigned int bufferSize,struct sockaddr_in *addr);
    int p2p_recv(SOCKET soc,P2P_TRAN_INFO_DATA *tranData,char *buffer,unsigned int bufferSize,struct sockaddr_in *addr);
private:
    int p2p_init_addr_list();
    int p2p_client_list_add_soc(char *toID,SOCKET soc);
    int p2p_client_list_search_addr(char *toID,struct sockaddr_in *addr);
    SOCKET p2p_connect_to_server(char *toID,char *fromID,struct sockaddr_in *addr,int flag);
    SOCKET p2p_create_connection(char *toID,char *fromID,struct sockaddr_in *toAddr,int flag);
private:
    P2P_CONN_LIST *P2P_Address_List_Header;
    CRITICAL_SECTION CRI_P2P_Addr_List;
};

#endif // P2P_H_INCLUDED








