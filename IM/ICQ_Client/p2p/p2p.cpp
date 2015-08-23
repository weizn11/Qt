#include "p2p.h"
#include "../commun/commun.h"

P2P gl_P2P;
extern ServerConn gl_Server_Conn_Info;

P2P::P2P()
{
    p2p_init_addr_list();
    srand(time(NULL));
    P2P_Address_List_Header=NULL;
}

P2P::~P2P()
{
    if(P2P_Address_List_Header!=NULL)
        free(P2P_Address_List_Header);
}

int P2P::p2p_init_addr_list()
{
    InitializeCriticalSection(&CRI_P2P_Addr_List);

    return 0;
}

int P2P::p2p_client_list_add_addr(char *toID,struct sockaddr_in addr)
{
    P2P_CONN_LIST *newNode=NULL,*pBody=NULL;

    newNode=(P2P_CONN_LIST *)malloc(sizeof(P2P_CONN_LIST));
    if(newNode==NULL)
    {
        //malloc error
        return -1;
    }
    memset(newNode,NULL,sizeof(P2P_CONN_LIST));

    EnterCriticalSection(&CRI_P2P_Addr_List);
    if(P2P_Address_List_Header==NULL)
        P2P_Address_List_Header=newNode;
    else
    {
        for(pBody=P2P_Address_List_Header; pBody->next!=NULL; pBody=pBody->next);
        pBody->next=newNode;
    }
    strcat(newNode->toID,toID);
    newNode->addr=addr;
    LeaveCriticalSection(&CRI_P2P_Addr_List);

    return 0;
}

int P2P::p2p_client_list_del_node(char *toID)
{
    P2P_CONN_LIST *pBody=NULL,*tmp=NULL;

    EnterCriticalSection(&CRI_P2P_Addr_List);
    pBody=P2P_Address_List_Header;
    if(pBody==NULL)
    {
        LeaveCriticalSection(&CRI_P2P_Addr_List);
        return -1;
    }
    for(; pBody!=NULL; pBody=pBody->next)
    {
        if(pBody==P2P_Address_List_Header)
        {
            if(!strcmp(pBody->toID,toID))
            {
                if(P2P_Address_List_Header->next!=NULL)
                {
                    tmp=P2P_Address_List_Header;
                    P2P_Address_List_Header=tmp->next;
                    free(tmp);
                    tmp=NULL;
                }
                else
                {
                    free(P2P_Address_List_Header);
                    P2P_Address_List_Header=NULL;
                }
                LeaveCriticalSection(&CRI_P2P_Addr_List);
                return 0;
            }
            else if(pBody->next!=NULL)
            {
                if(!strcmp(pBody->next->toID,toID))
                {
                    tmp=pBody->next;
                    pBody->next=pBody->next->next;
                    free(tmp);
                    tmp=NULL;
                    LeaveCriticalSection(&CRI_P2P_Addr_List);
                    return 0;
                }
            }
        }
    }

    LeaveCriticalSection(&CRI_P2P_Addr_List);

    return 0;
}

int P2P::p2p_client_list_search(char *toID,P2P_CONN_LIST **pP2PConnClient)
{
    P2P_CONN_LIST *pBody=NULL,*tmp=NULL;

    EnterCriticalSection(&CRI_P2P_Addr_List);
    pBody=P2P_Address_List_Header;
    if(pBody==NULL)
    {
        LeaveCriticalSection(&CRI_P2P_Addr_List);
        return -1;
    }
    for(; pBody!=NULL; pBody=pBody->next)
    {
        if(!strcmp(pBody->toID,toID))
        {
            if(pBody->effFlag==0)
            {
                LeaveCriticalSection(&CRI_P2P_Addr_List);
                return -3;
            }
            *pP2PConnClient=pBody;
            LeaveCriticalSection(&CRI_P2P_Addr_List);
            return 0;
        }
    }

    LeaveCriticalSection(&CRI_P2P_Addr_List);

    return -2;
}

int P2P::p2p_client_list_search_addr(char *toID,struct sockaddr_in *addr)
{
    P2P_CONN_LIST *pBody=NULL,*tmp=NULL;

    EnterCriticalSection(&CRI_P2P_Addr_List);
    pBody=P2P_Address_List_Header;
    if(pBody==NULL)
    {
        LeaveCriticalSection(&CRI_P2P_Addr_List);
        return -1;
    }
    for(; pBody!=NULL; pBody=pBody->next)
    {
        if(!strcmp(pBody->toID,toID))
        {
            *addr=pBody->addr;
            LeaveCriticalSection(&CRI_P2P_Addr_List);
            return 0;
        }
    }

    LeaveCriticalSection(&CRI_P2P_Addr_List);

    return -2;
}

int P2P::p2p_client_list_add_soc(char *toID,SOCKET soc)
{
    P2P_CONN_LIST *pBody=NULL,*tmp=NULL;

    EnterCriticalSection(&CRI_P2P_Addr_List);
    pBody=P2P_Address_List_Header;
    if(pBody==NULL)
    {
        LeaveCriticalSection(&CRI_P2P_Addr_List);
        return -1;
    }
    for(; pBody!=NULL; pBody=pBody->next)
    {
        printf("to:%s\n",toID);
        if(!strcmp(pBody->toID,toID))
        {
            pBody->soc=soc;
            pBody->effFlag=1;
            LeaveCriticalSection(&CRI_P2P_Addr_List);
            return 0;
        }
    }

    LeaveCriticalSection(&CRI_P2P_Addr_List);

    return -2;
}

SOCKET P2P::p2p_connect_to_server(char *toID,char *fromID,struct sockaddr_in *addr,int flag)
{
    P2P_CONN_INFO packet;
    SOCKET soc=INVALID_SOCKET;
    struct sockaddr_in serverAddr;
    P2P_TRAN_INFO_DATA tranData;

    memset(&serverAddr,NULL,sizeof(struct sockaddr_in));
    memset(&packet,NULL,sizeof(P2P_CONN_INFO));
    memset(&tranData,NULL,sizeof(P2P_TRAN_INFO_DATA));

    serverAddr.sin_family=AF_INET;
    serverAddr.sin_addr.s_addr=inet_addr(P2P_SERVER_IP);
    serverAddr.sin_port=htons(P2P_SERVER_PORT);

    strcat(packet.fromID,fromID);
    strcat(packet.toID,toID);
    packet.flag=flag;

    soc=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(soc==INVALID_SOCKET) return soc;

    //向服务端穿透
    if(sendto(soc,(char *)&packet,sizeof(P2P_CONN_INFO),0,(struct sockaddr *)&serverAddr,sizeof(struct sockaddr_in))<=0)
    {
        printf("UDP包发送失败.\n");
        closesocket(soc);
        return INVALID_SOCKET;
    }
    printf("服务端穿透成功\n");

    return soc;
}

SOCKET P2P::p2p_create_connection(char *toID,char *fromID,struct sockaddr_in *toAddr,int flag)
{
    //flag  1:发起连接方  0:被动连接方  toAddr:返回对方client地址
    int n;
    P2P_CONN_INFO packet,*pConnInfo=NULL;
    SOCKET soc=INVALID_SOCKET;
    struct sockaddr_in clientAddr,tmpAddr;
    struct timeval recvTimeout;
    P2P_TRAN_INFO_DATA tranData;
    char recvBuffer[P2P_DATA_MAXIMUM_SIZE];

    pConnInfo=(P2P_CONN_INFO *)recvBuffer;
    memset(&clientAddr,NULL,sizeof(struct sockaddr_in));
    memset(&packet,NULL,sizeof(P2P_CONN_INFO));
    memset(&tranData,NULL,sizeof(P2P_TRAN_INFO_DATA));
    memset(recvBuffer,NULL,sizeof(recvBuffer));
    memset(&recvTimeout,NULL,sizeof(struct timeval));

    //向服务端穿透
    soc=p2p_connect_to_server(toID,fromID,toAddr,flag);
    if(soc==INVALID_SOCKET)
        return soc;

    printf("Connect to P2P Server successfully\n");

    for(n=0; n<10; n++)
    {
        Sleep(500);
        if(p2p_client_list_search_addr(toID,&clientAddr)==0)
            break;
    }
    if(n>=10)
    {
        //timeout
        printf("Get Clinet addr timeout\n");
        p2p_client_list_del_node(toID);
        closesocket(soc);
        return INVALID_SOCKET;
    }

    *toAddr=clientAddr;
    return soc;
}

int P2P::p2p_create_connection_thread(P2P_CREATE_CONN_THREAD_PARA *threadPara)
{
    P2PCreateConnThread *createConnThread=NULL;

    createConnThread=new P2PCreateConnThread();
    if(createConnThread==NULL)
    {
        cout <<"new object error."<<endl;
        return -1;
    }
    threadPara->pP2PClass=this;
    createConnThread->run((void *)threadPara);

    return 0;
}

int P2P::p2p_send(SOCKET soc,P2P_TRAN_INFO_DATA *tranData,char *buffer,unsigned int bufferSize,struct sockaddr_in *addr)
{
    int i,addrLength;
    int sendSize,recvSize;
    int recvTimeout;
    struct sockaddr_in clientAddress;
    P2P_HEADER *pSendHeader=NULL,*pRecvHeader=NULL;
    char recvBuffer[UDP_BUFFER_MAXIMUM_SIZE],sendBuffer[UDP_BUFFER_MAXIMUM_SIZE];

    pSendHeader=(P2P_HEADER *)sendBuffer;
    pRecvHeader=(P2P_HEADER *)recvBuffer;

    pSendHeader->acknowledgementNum=++tranData->preSendAcknowledgemwntNum;
    memcpy(&sendBuffer[sizeof(P2P_HEADER)],buffer,bufferSize);

    recvTimeout=P2P_RECV_TIMEOUT*1000;
    setsockopt(soc,SOL_SOCKET,SO_RCVTIMEO,(char *)&recvTimeout,sizeof(recvTimeout));

    for(i=0; i<P2P_RESEND_OF_TIMES; i++)
    {
        if((sendSize=sendto(soc,sendBuffer,bufferSize+sizeof(P2P_HEADER),0,(struct sockaddr *)addr,sizeof(struct sockaddr_in)))<=0)
            return sendSize;
skip_recv:
        memset(recvBuffer,NULL,sizeof(recvBuffer));
        memset(&clientAddress,NULL,sizeof(clientAddress));
        addrLength=sizeof(struct sockaddr_in);
        //接收确认号
        if((recvSize=recvfrom(soc,recvBuffer,sizeof(recvBuffer),0,(struct sockaddr *)&clientAddress,&addrLength))<=0)
        {
            //printf("超时重发.\n");
            if(WSAGetLastError()==WSAETIMEDOUT)
                continue;        //超时重新发送数据包
            return recvSize;
        }
        //printf("接收到的确认号:%d\n",pRecvHeader->acknowledgementNum);
        if(pRecvHeader->acknowledgementNum!=pSendHeader->acknowledgementNum)
            goto skip_recv;
        else
            break;
    }
    recvTimeout=0;
    setsockopt(soc,SOL_SOCKET,SO_RCVTIMEO,(char *)&recvTimeout,sizeof(recvTimeout));
    sendSize-=sizeof(P2P_HEADER);

    return sendSize;
}

int P2P::p2p_recv(SOCKET soc,P2P_TRAN_INFO_DATA *tranData,char *buffer,unsigned int bufferSize,struct sockaddr_in *addr)
{
    //返回接收到的数据长度,tranData首次传入时需要清空
    int addrLength;
    int recvSize;

    P2P_HEADER *pSendHeader=NULL,*pRecvHeader=NULL;
    char recvBuffer[UDP_BUFFER_MAXIMUM_SIZE],sendBuffer[UDP_BUFFER_MAXIMUM_SIZE];

    pSendHeader=(P2P_HEADER *)sendBuffer;
    pRecvHeader=(P2P_HEADER *)recvBuffer;
    memset(buffer,NULL,bufferSize);

skip_recv:
    memset(addr,NULL,sizeof(struct sockaddr_in));
    memset(recvBuffer,NULL,sizeof(recvBuffer));
    addrLength=sizeof(struct sockaddr_in);
    recvSize=recvfrom(soc,recvBuffer,sizeof(recvBuffer),0,(struct sockaddr *)addr,&addrLength);
    if(recvSize<=0)
    {
        printf("recvfrom error.\n");
        return recvSize;
    }
    //printf("recvfrom size:%d\n",recvSize);

    memset(sendBuffer,NULL,sizeof(sendBuffer));

    pSendHeader->acknowledgementNum=pRecvHeader->acknowledgementNum;
    sendto(soc,sendBuffer,sizeof(P2P_HEADER),0,(struct sockaddr *)addr,sizeof(struct sockaddr_in));

    if(tranData->preRecvAcknowledgemwntNum>=pRecvHeader->acknowledgementNum)
    {
        printf("Packet repeat.\n");
        goto skip_recv;
    }
    tranData->preRecvAcknowledgemwntNum=pRecvHeader->acknowledgementNum;
    recvSize-=sizeof(P2P_HEADER);
    memcpy(buffer,&recvBuffer[sizeof(P2P_HEADER)],recvSize);

    return recvSize;
}

/*
int P2P::p2p_send(SOCKET soc,char *buffer,unsigned int bufferSize,struct sockaddr_in *addr)
{
    //tranData首次传入时需要清空
    int i,addrLength;
    int sendSize,recvSize;
    unsigned long int acknowledgementNum;
    int recvTimeout;
    struct sockaddr_in clientAddress;
    P2P_HEADER *pSendHeader=NULL,*pRecvHeader=NULL;
    char recvBuffer[UDP_BUFFER_MAXIMUM_SIZE],sendBuffer[UDP_BUFFER_MAXIMUM_SIZE];

    pSendHeader=(P2P_HEADER *)sendBuffer;
    pRecvHeader=(P2P_HEADER *)recvBuffer;

    if(tranData->initFlag==0)
    {
        srand(GetTickCount());
        tranData->initFlag=1;
    }
    do
    {
        acknowledgementNum=pSendHeader->acknowledgementNum=rand();
        if(tranData->preSendAcknowledgemwntNum==0)
        {
            tranData->preSendAcknowledgemwntNum=acknowledgementNum;
            break;
        }
    }
    while(acknowledgementNum==tranData->preSendAcknowledgemwntNum);

    //printf("计算出的确认号:%d\n",acknowledgementNum);

    memcpy(&sendBuffer[sizeof(P2P_HEADER)],buffer,bufferSize);

    recvTimeout=P2P_RECV_TIMEOUT*1000;
    setsockopt(soc,SOL_SOCKET,SO_RCVTIMEO,(char *)&recvTimeout,sizeof(recvTimeout));

    for(i=0; i<P2P_RESEND_OF_TIMES; i++)
    {
        if((sendSize=sendto(soc,sendBuffer,bufferSize+sizeof(P2P_HEADER),0,(struct sockaddr *)addr,sizeof(struct sockaddr_in)))<=0)
            return sendSize;
skip_recv:
        memset(recvBuffer,NULL,sizeof(recvBuffer));
        memset(&clientAddress,NULL,sizeof(clientAddress));
        addrLength=sizeof(struct sockaddr_in);
        //接收确认号
        if((recvSize=recvfrom(soc,recvBuffer,sizeof(recvBuffer),0,(struct sockaddr *)&clientAddress,&addrLength))<=0)
        {
            //printf("超时重发.\n");
            if(WSAGetLastError()==WSAETIMEDOUT)
                continue;        //超时重新发送数据包
            return recvSize;
        }
        //printf("接收到的确认号:%d\n",pRecvHeader->acknowledgementNum);
        if(pRecvHeader->acknowledgementNum!=acknowledgementNum)
            goto skip_recv;
        else
            break;
    }
    recvTimeout=0;
    setsockopt(soc,SOL_SOCKET,SO_RCVTIMEO,(char *)&recvTimeout,sizeof(recvTimeout));
    sendSize-=sizeof(P2P_HEADER);

    return sendSize;
}

int P2P::p2p_recv(SOCKET soc,char *buffer,unsigned int bufferSize,struct sockaddr_in *addr)
{
    //返回接收到的数据长度,tranData首次传入时需要清空
    int addrLength;
    int sendSize,recvSize;

    unsigned long int acknowledgementNum;
    P2P_HEADER *pSendHeader=NULL,*pRecvHeader=NULL;
    char recvBuffer[UDP_BUFFER_MAXIMUM_SIZE],sendBuffer[UDP_BUFFER_MAXIMUM_SIZE];

    pSendHeader=(P2P_HEADER *)sendBuffer;
    pRecvHeader=(P2P_HEADER *)recvBuffer;
    memset(buffer,NULL,bufferSize);

skip_recv:
    memset(addr,NULL,sizeof(struct sockaddr_in));
    memset(recvBuffer,NULL,sizeof(recvBuffer));
    addrLength=sizeof(struct sockaddr_in);
    recvSize=recvfrom(soc,recvBuffer,sizeof(recvBuffer),0,(struct sockaddr *)addr,&addrLength);
    if(recvSize<=0)
    {
        printf("recvfrom error.\n");
        return recvSize;
    }
    //printf("recvfrom size:%d\n",recvSize);

    memset(sendBuffer,NULL,sizeof(sendBuffer));
    pSendHeader->acknowledgementNum=pRecvHeader->acknowledgementNum;
    sendto(soc,sendBuffer,sizeof(P2P_HEADER),0,(struct sockaddr *)addr,sizeof(struct sockaddr_in));

    if(pRecvHeader->acknowledgementNum==tranData->preRecvAcknowledgemwntNum)
    {
        printf("Packet repeat.\n");
        goto skip_recv;
    }
    tranData->preRecvAcknowledgemwntNum=pRecvHeader->acknowledgementNum;
    recvSize-=sizeof(P2P_HEADER);
    memcpy(buffer,&recvBuffer[sizeof(P2P_HEADER)],recvSize);

    return recvSize;
}
*/

int P2PCreateConnThread::func(void *threadPara)
{
    P2P_CREATE_CONN_THREAD_PARA *pThreadPara=(P2P_CREATE_CONN_THREAD_PARA *)threadPara;
    SOCKET soc;
    int i,addrLength;
    struct sockaddr_in toAddr,tmpAddr;
    int recvTimeout,passFlag;
    P2P_PACKET p2pPacket;

    memset(&toAddr,NULL,sizeof(struct sockaddr_in));

    if(pThreadPara->P2PConnInfo.flag==1)
    {
        //向服务端发送TCP请求
        if(gl_Server_Conn_Info.ex_send((char *)&pThreadPara->P2PConnInfo,sizeof(P2P_CONN_INFO),1)<=0)
        {
            free(pThreadPara);
            return -1;
        }

        printf("TCP request successfully\n");
    }

    soc=pThreadPara->pP2PClass->p2p_create_connection(pThreadPara->P2PConnInfo.toID,\
                                                      pThreadPara->P2PConnInfo.fromID,\
                                                      &toAddr,pThreadPara->P2PConnInfo.flag);
    if(soc==INVALID_SOCKET)
    {
        free(pThreadPara);
        pThreadPara->pP2PClass->p2p_client_list_del_node(pThreadPara->P2PConnInfo.toID);
        return soc;
    }

    //向另一个端点穿透
    recvTimeout=P2P_RECV_TIMEOUT*1000;
    setsockopt(soc,SOL_SOCKET,SO_RCVTIMEO,(char *)&recvTimeout, sizeof(recvTimeout));
    for(i=passFlag=0; i<P2P_RESEND_OF_TIMES; i++)
    {
        printf("向%s:%d发送穿透包\n",inet_ntoa(toAddr.sin_addr),ntohs(toAddr.sin_port));
        memset(&p2pPacket,NULL,sizeof(P2P_PACKET));
        p2pPacket.proto=1;
        strcat(p2pPacket.data,P2P_ACKNOWLEDGMENT_NUM);
        if(sendto(soc,(char *)&p2pPacket,sizeof(P2P_PACKET),0,(struct sockaddr *)&toAddr,sizeof(struct sockaddr_in))<=0)
        {
            printf("sendto failed.error code:%d\n",WSAGetLastError());
        }
skip_again:
        memset(&p2pPacket,NULL,sizeof(P2P_PACKET));
        memset(&tmpAddr,NULL,sizeof(struct sockaddr_in));
        addrLength=sizeof(struct sockaddr_in);
        if(recvfrom(soc,(char *)&p2pPacket,sizeof(P2P_PACKET),0,(struct sockaddr *)&tmpAddr,&addrLength)<=0)
        {
            if(passFlag)
                goto skip_over;
            printf("recvfrom failed.error code:%d\n",WSAGetLastError());
            continue;
        }
        p2pPacket.data[PROTO_DATA_MAX_SIZE-1]=NULL;
        if(!strcmp(p2pPacket.data,P2P_ACKNOWLEDGMENT_NUM))
        {
            printf("收到穿透包.from %s:%d\n",inet_ntoa(tmpAddr.sin_addr),ntohs(tmpAddr.sin_port));
            passFlag=1;
            goto skip_again;
        }
    }
skip_over:
    recvTimeout=0;
    setsockopt(soc,SOL_SOCKET,SO_RCVTIMEO,(char *)&recvTimeout, sizeof(recvTimeout));

    if(i>=P2P_RESEND_OF_TIMES)
    {
        printf("穿透失败\n");
        pThreadPara->pP2PClass->p2p_client_list_del_node(pThreadPara->P2PConnInfo.toID);
        free(pThreadPara);
        return -1;
    }

    printf("穿透成功\n");
    pThreadPara->pP2PClass->p2p_client_list_add_soc(pThreadPara->P2PConnInfo.toID,soc);
    free(pThreadPara);
    pThreadPara=NULL;
    return 0;
}

P2PCreateConnThread::~P2PCreateConnThread()
{
    delete this;
}




