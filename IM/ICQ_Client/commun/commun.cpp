#include "commun.h"
#include "friendstreeview.h"
#include "chatwindow.h"
#include "mainwindow.h"
#include "groupslistview.h"

extern char SERVER_IP[16];
extern P2P gl_P2P;
extern ServerConn gl_Server_Conn_Info;
RECV_SIGNAL gl_Recv_Signal;
extern FriendTreeOperation *gl_FriendTree_Operation;
extern ChatWindow *glp_ChatWindow;
extern MainWindow *pMainWindow;
extern GroupListOperation *glp_GroupListOperation;
RecvThread *recvThread=NULL;

RecvThread::RecvThread(QObject *parent) :
    QThread(parent)
{
    recvThread=this;
    qRegisterMetaType<SMS_DATA>("SMS_DATA");

    connect(recvThread,SIGNAL(signal_add_sms_unread(SMS_DATA)),gl_FriendTree_Operation,SLOT(slot_add_sms_unread(SMS_DATA)));
    connect(recvThread,SIGNAL(signal_add_sms_log(SMS_DATA)),gl_FriendTree_Operation,SLOT(slot_add_sms_log(SMS_DATA)));
    connect(recvThread,SIGNAL(signal_delete_friend(char*)),gl_FriendTree_Operation,SLOT(slot_delete_friend(char*)));
    connect(recvThread,SIGNAL(signal_add_sms_log(GROUP_SMS_DATA)),glp_GroupListOperation,SLOT(slot_add_sms_log(GROUP_SMS_DATA)));

    return;
}

void RecvThread::run()
{
    KeepAliveThread gl_KeepAlive;

    gl_KeepAlive.run(NULL);
    recv_data_from_server();
    return;
}

int RecvThread::recv_icon(ICON_TRAN_DATA *pIconData)
{
    char str[1000];
    char *num=NULL;
    FriendItemWidget *friendItemWidget=NULL;

    memset(str,NULL,sizeof(str));

    strcat(str,gl_Server_Conn_Info.tmpPath);
    strcat(str,"/");
    strcat(str,pIconData->num);
    strcat(str,".jpg");

    if(strcmp(pIconData->num,gl_Server_Conn_Info.accountInfo.num))
    {
        friendItemWidget=gl_FriendTree_Operation->get_item_widget_by_num(pIconData->num);
        if(friendItemWidget==NULL)
        {
            cout <<pIconData->num<<"FriendItemWidget is empty."<<endl;
            return -1;
        }
        if(pIconData->flushFlag)
        {
            friendItemWidget->pFile=fopen(str,"wb");
            if(friendItemWidget->pFile==NULL)
            {
                cout <<"open file error"<<endl;
                return -1;
            }
        }
        cout <<"icon data:"<<pIconData->num<<endl;
        fwrite(pIconData->data,pIconData->dataSize,1,friendItemWidget->pFile);
        fflush(friendItemWidget->pFile);
        if(pIconData->flag==0)
        {
            cout <<"update '"<<num<<"' icon"<<endl;
            fclose(friendItemWidget->pFile);
            friendItemWidget->pFile=NULL;
            num=(char *)malloc(sizeof(pIconData->num));
            memset(num,NULL,sizeof(pIconData->num));
            strcat(num,pIconData->num);
            signal_update_item_icon(num);
        }
    }
    else
    {
        qDebug() <<"recv myself icon";
        if(gl_Server_Conn_Info.pFile==NULL)
        {
            gl_Server_Conn_Info.pFile=fopen(gl_Server_Conn_Info.iconPath,"wb");
            if(gl_Server_Conn_Info.pFile==NULL)
            {
                qDebug() <<"create icon file error";
                qDebug() <<"The path:"<<gl_Server_Conn_Info.iconPath;
                return -1;
            }
        }
        fwrite(pIconData->data,pIconData->dataSize,1,gl_Server_Conn_Info.pFile);
        fflush(gl_Server_Conn_Info.pFile);
        if(pIconData->flag==0)
        {
            fclose(gl_Server_Conn_Info.pFile);
            gl_Server_Conn_Info.pFile=NULL;
            qDebug() <<"update_myself_info";
            signal_update_myself_icon();
        }
    }

    return 0;
}

int RecvThread::recv_data_from_server()
{
    int recvSize;
    void *pThreadPara=NULL;
    MAIN_PACKET *packet=NULL;
    P2P_CONN_INFO *pP2PConnInfo=NULL;
    ServerConn *pServerConnInfo=(ServerConn *)&gl_Server_Conn_Info;
    ChatItemWidget *pChatItemWidget=NULL;
    char iconPath[255];
    FILE_TRAN_DATA *pFileTranData=NULL;
    vector<FILE_TRAN_LIST>::iterator iterFileTranList;
    char str[1000],*p=NULL;


    printf("recv thread start...\n");

    while(1)
    {
        if(pServerConnInfo->recvSize==0)
            memset(pServerConnInfo->recvBuffer,NULL,sizeof(pServerConnInfo->recvBuffer));
        recvSize=recv(pServerConnInfo->soc,&pServerConnInfo->recvBuffer[pServerConnInfo->recvSize],\
                      sizeof(MAIN_PACKET)-pServerConnInfo->recvSize,0);
        printf("recvSize:%d\n",recvSize);
        if(recvSize<=0)
        {
            printf("recv failed!\n");
            pServerConnInfo->recvSize=0;
            memset(pServerConnInfo->recvBuffer,NULL,sizeof(pServerConnInfo->recvBuffer));
            return -1;
        }
        pServerConnInfo->recvSize+=recvSize;
        if(pServerConnInfo->recvSize==sizeof(MAIN_PACKET))
        {
            packet=(MAIN_PACKET *)pServerConnInfo->recvBuffer;
            cout <<"recv proto:"<<packet->proto<<endl;
            switch(packet->proto)
            {
            case 1:
                qDebug() <<"recv p2p connect request";
                pP2PConnInfo=(P2P_CONN_INFO *)packet->data;
                if(pP2PConnInfo->flag==1)
                {
                    pThreadPara=(void *)malloc(sizeof(P2P_CREATE_CONN_THREAD_PARA));
                    if(pThreadPara==NULL)
                        break;
                    memset(pThreadPara,NULL,sizeof(P2P_CREATE_CONN_THREAD_PARA));
                    pP2PConnInfo->flag=0;
                    memcpy((char *)&(((P2P_CREATE_CONN_THREAD_PARA *)pThreadPara)->P2PConnInfo),pP2PConnInfo,\
                           sizeof(P2P_CONN_INFO));
                    gl_P2P.p2p_create_connection_thread((P2P_CREATE_CONN_THREAD_PARA *)pThreadPara);

                    gl_P2P.p2p_client_list_add_addr(pP2PConnInfo->toID,pP2PConnInfo->toAddr);
                    sprintf(str,"toID:%s\t%s:%d",pP2PConnInfo->toID,inet_ntoa(pP2PConnInfo->toAddr.sin_addr),\
                           ntohs(pP2PConnInfo->toAddr.sin_port));
                    qDebug() <<str;
                }
                else
                {
                    gl_P2P.p2p_client_list_add_addr(pP2PConnInfo->toID,pP2PConnInfo->toAddr);
                    sprintf(str,"toID:%s\t%s:%d",pP2PConnInfo->toID,inet_ntoa(pP2PConnInfo->toAddr.sin_addr),\
                           ntohs(pP2PConnInfo->toAddr.sin_port));
                    qDebug() <<str;
                }
                break;
            case 2:
                memset(&gl_Recv_Signal,NULL,sizeof(gl_Recv_Signal));
                memcpy(&gl_Recv_Signal,packet,sizeof(gl_Recv_Signal));
                if(((ACCOUNT_INFO_DATA *)packet->data)->success==1)
                {
                    strcat(gl_Server_Conn_Info.iconPath,((ACCOUNT_INFO_DATA *)packet->data)->num);
                    strcat(gl_Server_Conn_Info.iconPath,".jpg");
                }
                break;
            case 3:
                memset(&gl_Recv_Signal,NULL,sizeof(gl_Recv_Signal));
                memcpy(&gl_Recv_Signal,packet,sizeof(gl_Recv_Signal));
                break;
            case 4:
                memset(&gl_Recv_Signal,NULL,sizeof(MAIN_PACKET));
                memcpy(&gl_Recv_Signal,packet,sizeof(MAIN_PACKET));
                break;
            case 5:
                memset(&gl_Recv_Signal,NULL,sizeof(MAIN_PACKET));
                memcpy(&gl_Recv_Signal,packet,sizeof(MAIN_PACKET));
                break;
            case 6:
                cout <<"send signal_update_friend_item"<<endl;
                memset(iconPath,NULL,sizeof(iconPath));
                strcat(iconPath,gl_Server_Conn_Info.tmpPath);
                strcat(iconPath,((FRIEND_INFO_DATA *)packet->data)->friendInfo.num);
                strcat(iconPath,".jpg");
                //remove(iconPath);
                signal_update_friend_item(*(FRIEND_INFO_DATA *)packet->data);
                Sleep(500);
                break;
            case 7:
                memset(&gl_Recv_Signal,NULL,sizeof(MAIN_PACKET));
                memcpy(&gl_Recv_Signal,packet,sizeof(MAIN_PACKET));
                break;
            case 8:
                recv_icon((ICON_TRAN_DATA *)packet->data);
                break;
            case 9:
                cout <<"The sms come from:"<<((SMS_DATA *)packet->data)->fromNum<<endl;
                pChatItemWidget=glp_ChatWindow->search_chat_item_by_num(((SMS_DATA *)packet->data)->fromNum);
                if(pChatItemWidget==NULL)
                {
                    //对话窗口未打开
                    cout <<"signal_add_sms_unread"<<endl;
                    signal_add_sms_unread(*((SMS_DATA *)packet->data));
                }
                else
                {
                    cout <<"signal_show_sms_on_text"<<endl;
                    signal_show_sms_on_text(*((SMS_DATA *)packet->data));
                    signal_add_sms_log(*((SMS_DATA *)packet->data));
                }
                break;
            case 10:
                pFileTranData=(FILE_TRAN_DATA *)packet->data;
                switch(pFileTranData->auth)
                {
                case 0:
                    signal_recv_file(*pFileTranData);
                    break;
                case -1:
                    signal_reject_recv_file(*pFileTranData);
                    break;
                case 1:
                    signal_start_tran_file(*pFileTranData);
                    break;
                case 2:
                    for(iterFileTranList=gl_Server_Conn_Info.vectorFileRecvList.begin();\
                        iterFileTranList!=gl_Server_Conn_Info.vectorFileRecvList.end();iterFileTranList++)
                    {
                        if((*iterFileTranList).fileTranData.sequenceNum==pFileTranData->sequenceNum)
                        {
                            fwrite(pFileTranData->fileData,pFileTranData->dataSize,1,(*iterFileTranList).pFile);
                            fflush((*iterFileTranList).pFile);
                            (*iterFileTranList).tranDoneSize+=pFileTranData->dataSize;
                            signal_update_recv_info((*iterFileTranList).tranDoneSize,pFileTranData->fileSize,pFileTranData->fromNum);
                            if(pFileTranData->continueFlag==0)
                            {
                                //文件接收完成
                                fclose((*iterFileTranList).pFile);
                                signal_update_recv_info(pFileTranData->fileSize,pFileTranData->fileSize,pFileTranData->fromNum);
                            }
                            break;
                        }
                    }
                    break;
                }
                break;
            case 11:
                memset(str,NULL,sizeof(str));
                strcat(str,((FRIENDSHIP_INFO_DATA *)packet->data)->num1);
                signal_delete_friend(str);
                break;
            case 12:
                memset(&gl_Recv_Signal,NULL,sizeof(MAIN_PACKET));
                memcpy(&gl_Recv_Signal,packet,sizeof(MAIN_PACKET));
                break;
            case 13:
                memset(&gl_Recv_Signal,NULL,sizeof(MAIN_PACKET));
                memcpy(&gl_Recv_Signal,packet,sizeof(MAIN_PACKET));
                break;
            case 14:
                memset(&gl_Recv_Signal,NULL,sizeof(MAIN_PACKET));
                memcpy(&gl_Recv_Signal,packet,sizeof(MAIN_PACKET));
                break;
            case 15:
                signal_update_group_list_item(*((GROUP_INFO_DATA *)packet->data));
                break;
            case 16:
                pChatItemWidget=glp_ChatWindow->search_group_chat_item_by_num(((GROUP_SMS_DATA *)packet->data)->toGroupNum);
                if(pChatItemWidget==NULL)
                {
                    //对话窗口未打开
                }
                else
                {
                    cout <<"signal_show_sms_on_text"<<endl;
                    signal_show_sms_on_text(*((GROUP_SMS_DATA *)packet->data));
                }
                signal_add_sms_log(*((GROUP_SMS_DATA *)packet->data));
                break;
            default:
                break;
            }
            memset(pServerConnInfo->recvBuffer,NULL,sizeof(pServerConnInfo->recvBuffer));
            pServerConnInfo->recvSize=0;
        }
    }

    return 0;
}

ServerConn::ServerConn()
{
    char *p=NULL;

    soc=INVALID_SOCKET;
    memset(ID,NULL,sizeof(ID));
    memset(&addr,NULL,sizeof(struct sockaddr_in));
    memset(recvBuffer,NULL,sizeof(recvBuffer));
    InitializeCriticalSection(&CRI_Send);
    memset(&gl_Recv_Signal,NULL,sizeof(RECV_SIGNAL));
    memset(&accountInfo,NULL,sizeof(accountInfo));
    memset(iconPath,NULL,sizeof(iconPath));
    mkdir("D:\\ICQ_ICON");
    strcat(iconPath,"D:/ICQ_ICON");
    for(p=iconPath;*p!=NULL;p++)
    {
        if(*p=='\\')
            *p='/';
    }
    strcat(iconPath,"/");
    memset(tmpPath,NULL,sizeof(tmpPath));
    strcat(tmpPath,iconPath);
}

ServerConn::~ServerConn()
{
    ;
}

int ServerConn::auth_account(char *num,char *password)
{
    int i;

    memset(&accountInfo,NULL,sizeof(accountInfo));
    strcat(accountInfo.num,num);
    strcat(accountInfo.password,password);

    if(ex_send((char *)&accountInfo,sizeof(accountInfo),2)<=0)
    {
        printf("网络连接错误.\n");
        closesocket(gl_Server_Conn_Info.soc);
        gl_Server_Conn_Info.soc=INVALID_SOCKET;
        return -1;
    }

    for(i=0;i<=20;i++)
    {
        if(i>=20) return -2;      //timeout
        if(gl_Recv_Signal.proto==2)
            break;
        Sleep(500);
    }
    accountInfo=*(ACCOUNT_INFO_DATA *)gl_Recv_Signal.data;
    memset(&gl_Recv_Signal,NULL,sizeof(gl_Recv_Signal));
    if(accountInfo.success!=1)
    {
        closesocket(gl_Server_Conn_Info.soc);
        gl_Server_Conn_Info.soc=INVALID_SOCKET;
        if(accountInfo.success==-1)
            return -1;  //已经登录
        return -3;     //认证失败
    }

    return 0;
}

int ServerConn::update_account_data(ACCOUNT_INFO_DATA *pAccountData)
{
    int i;

    if(ex_send((char *)pAccountData,sizeof(ACCOUNT_INFO_DATA),7)<=0)
    {
        return -1;
    }

    for(i=0;i<20;i++)
    {
        Sleep(500);
        if(gl_Recv_Signal.proto==7)
        {
            if(((ACCOUNT_INFO_DATA *)gl_Recv_Signal.data)->success==1)
            {
                gl_Server_Conn_Info.accountInfo=*((ACCOUNT_INFO_DATA *)gl_Recv_Signal.data);
                memset(&gl_Recv_Signal,NULL,sizeof(gl_Recv_Signal));
                return 0;
            }
            else
                return -2;
        }
    }

    return -3;
}

int ServerConn::register_account(REGISTER_ACCOUNT_DATA *pRegData)
{
    int i;

    if(connect_to_server()!=0)
    {
        cout <<"网络连接错误"<<endl;
        return -1;
    }
    if(ex_send((char *)pRegData,sizeof(REGISTER_ACCOUNT_DATA),3)<=0)
    {
        cout <<"与服务器断开连接"<<endl;
        return -2;
    }

    for(i=0;i<=20;i++)
    {
        if(i>=20) return -2;      //timeout
        if(gl_Recv_Signal.proto==3)
            break;
        Sleep(500);
    }
    *pRegData=*(REGISTER_ACCOUNT_DATA *)gl_Recv_Signal.data;
    memset(&gl_Recv_Signal,NULL,sizeof(gl_Recv_Signal));
    if(pRegData->success!=1)
    {
        closesocket(gl_Server_Conn_Info.soc);
        gl_Server_Conn_Info.soc=INVALID_SOCKET;
        return -3;
    }
    closesocket(gl_Server_Conn_Info.soc);
    gl_Server_Conn_Info.soc=INVALID_SOCKET;

    return 0;
}

int ServerConn::connect_to_server()
{
    WSADATA wsa;

    memset(&wsa,NULL,sizeof(WSADATA));

    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(SERVER_IP);
    addr.sin_port=htons(SERVER_PORT);

    if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
    {
        printf("WSAStartup error.\n");
        return -1;
    }

    soc=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(soc==INVALID_SOCKET)
    {
        printf("create socket failed.\n");
        return -2;
    }

    if(connect(soc,(struct sockaddr *)&addr,sizeof(struct sockaddr_in))!=0)
    {
        printf("connect failed.\n");
        return -2;
    }
    recvThread->start();
    Sleep(100);

    return 0;
}

int ServerConn::ex_send(char *sendBuffer,unsigned int bufferSize,int proto)
{
    int sendSize;
    MAIN_PACKET Packet;

    memset(&Packet,NULL,sizeof(MAIN_PACKET));

    Packet.proto=proto;
    memcpy(Packet.data,sendBuffer,bufferSize);

    EnterCriticalSection(&CRI_Send);
    sendSize=send(soc,(char *)&Packet,sizeof(MAIN_PACKET),0);
    LeaveCriticalSection(&CRI_Send);

    return sendSize;
}

int ServerConn::find_friend(ACCOUNT_INFO_DATA *pAccountInfo)
{
    //proto:4
    int i;

    if(ex_send((char *)pAccountInfo,sizeof(ACCOUNT_INFO_DATA),4)<=0)
    {
        //网络连接错误
        return -1;
    }

    for(i=0;i<=20;i++)
    {
        if(i>=20) return -2;      //timeout
        if(gl_Recv_Signal.proto==4 && !strcmp(((ACCOUNT_INFO_DATA *)(gl_Recv_Signal.data))->num,pAccountInfo->num))
            break;
        Sleep(500);
    }

    *pAccountInfo=*(ACCOUNT_INFO_DATA *)gl_Recv_Signal.data;
    memset(&gl_Recv_Signal,NULL,sizeof(RECV_SIGNAL));
    if(pAccountInfo->success==0)
        return -3;      //不存在此账号

    return 0;
}

int ServerConn::add_friend(FRIENDSHIP_INFO_DATA *pFriendshipInfo)
{
    //proto:5
    int i;

    if(ex_send((char *)pFriendshipInfo,sizeof(FRIENDSHIP_INFO_DATA),5)<=0)
    {
        //网络连接错误
        return -1;
    }

    for(i=0;i<=20;i++)
    {
        if(i>=20) return -2;      //timeout
        if(gl_Recv_Signal.proto==5)
            break;
        Sleep(500);
    }

    *pFriendshipInfo=*(FRIENDSHIP_INFO_DATA *)gl_Recv_Signal.data;
    memset(&gl_Recv_Signal,NULL,sizeof(RECV_SIGNAL));
    if(pFriendshipInfo->success==-2)
        return -2;      //添加好友失败，原因：系统异常！
    else if(pFriendshipInfo->success==-3)
        return -3;  //添加好友失败，原因：你们已经是好友！

    return 0;
}

int ServerConn::upload_icon(char *pFilePath)
{
    FILE *pFileIcon=NULL;
    ICON_TRAN_DATA iconData;

    if((pFileIcon=fopen(pFilePath,"rb"))==NULL)
    {
        return -1;
    }
    do
    {
        memset(&iconData,NULL,sizeof(iconData));
        iconData.dataSize=fread(iconData.data,sizeof(char),sizeof(iconData.data),pFileIcon);
        if(feof(pFileIcon))
            iconData.flag=0;
        else
            iconData.flag=1;
        strcat(iconData.num,gl_Server_Conn_Info.accountInfo.num);
        ex_send((char *)&iconData,sizeof(iconData),8);
    }while(iconData.flag);
    fclose(pFileIcon);

    return 0;
}

int ServerConn::send_sms(SMS_DATA *pSMSData)
{
    if(ex_send((char *)pSMSData,sizeof(SMS_DATA),9)<1)
        return -1;

    return 0;
}

int ServerConn::send_sms(GROUP_SMS_DATA *pGroupSMSData)
{
    if(ex_send((char *)pGroupSMSData,sizeof(GROUP_SMS_DATA),16)<1)
        return -1;

    return 0;
}

char *ServerConn::current_time_string()
{
    struct tm currentTimeStru;
    time_t currentSec;
    char *currentTimeChar=NULL;

    currentTimeChar=(char *)malloc(100);
    if(currentTimeChar==NULL) return NULL;
    memset(currentTimeChar,NULL,100);
    currentSec=time(NULL);
    currentTimeStru=*localtime(&currentSec);\
    sprintf(currentTimeChar,"%d-%d-%d %02d:%02d:%02d",currentTimeStru.tm_year+1900,currentTimeStru.tm_mon+1,currentTimeStru.tm_mday,\
            currentTimeStru.tm_hour,currentTimeStru.tm_min,currentTimeStru.tm_sec);

    return currentTimeChar;
}

int ServerConn::add_file_tran_list(FILE_TRAN_LIST fileTranList)
{
    fileTranList.pFile=fopen(fileTranList.filePath,"rb");
    if(fileTranList.pFile==NULL)
    {
        cout <<"打开文件失败"<<endl;
        return -1;
    }
    strcat(fileTranList.fileTranData.fromNum,gl_Server_Conn_Info.accountInfo.num);
    fileTranList.fileTranData.sequenceNum=GetTickCount();
    fseek(fileTranList.pFile,0,SEEK_END);
    fileTranList.fileTranData.fileSize=ftell(fileTranList.pFile);
    fclose(fileTranList.pFile);
    if(ex_send((char *)&fileTranList.fileTranData,sizeof(FILE_TRAN_DATA),10)<1)
    {
        cout <<"网络连接错误"<<endl;
        return -2;
    }

    vectorFileTranList.push_back(fileTranList);

    return 0;
}

int ServerConn::delete_friend(char *num)
{
    FRIENDSHIP_INFO_DATA friendshipInfoData;

    memset(&friendshipInfoData,NULL,sizeof(friendshipInfoData));

    strcat(friendshipInfoData.num1,accountInfo.num);
    strcat(friendshipInfoData.num2,num);

    ex_send((char *)&friendshipInfoData,sizeof(friendshipInfoData),11);

    return 0;
}

int ServerConn::create_group(CREATE_GROUP_DATA *pCreateGroupData)
{
    int n;

    if(ex_send((char *)pCreateGroupData,sizeof(CREATE_GROUP_DATA),12)<=0)
    {
        cout <<"网络连接错误"<<endl;
        return -1;
    }

    for(n=0;n<10;n++)
    {
        if(gl_Recv_Signal.proto==12)
        {
            if(((CREATE_GROUP_DATA *)gl_Recv_Signal.data)->flag==1)
            {
                cout <<"group create successful"<<endl;
                memcpy(pCreateGroupData->groupNum,((CREATE_GROUP_DATA *)gl_Recv_Signal.data)->groupNum,\
                       strlen(((CREATE_GROUP_DATA *)gl_Recv_Signal.data)->groupNum));
                memset(&gl_Recv_Signal,NULL,sizeof(gl_Recv_Signal));
                return 0;
            }
            else
            {
                cout <<"The system error."<<endl;
                memset(&gl_Recv_Signal,NULL,sizeof(gl_Recv_Signal));
                return -2;
            }
        }
        Sleep(500);
    }
    return -3;
}

int ServerConn::find_group(GROUP_INFO_DATA *pGroupInfoData)
{
    int n;

    if(ex_send((char *)pGroupInfoData,sizeof(GROUP_INFO_DATA),13)<=0)
    {
        cout <<"网络连接错误！"<<endl;
        return -1;
    }

    for(n=0;n<=5;n++)
    {
        if(gl_Recv_Signal.proto==13)
        {
            if(!strcmp(pGroupInfoData->groupNum,((GROUP_INFO_DATA *)&gl_Recv_Signal.data)->groupNum))
            {
                *pGroupInfoData=*((GROUP_INFO_DATA *)&gl_Recv_Signal.data);
                memset(&gl_Recv_Signal,NULL,sizeof(gl_Recv_Signal));
                if(pGroupInfoData->flag==1)
                    return 0;
                else if(pGroupInfoData->flag==0)
                    return -2;
                else
                    return -3;
            }
        }
        Sleep(1000);
    }

    return -4;
}

int ServerConn::join_group(GROUP_INFO_DATA *pGroupInfoData)
{
    int n;

    if(ex_send((char *)pGroupInfoData,sizeof(GROUP_INFO_DATA),14)<=0)
    {
        qDebug() <<"网络连接错误";
        return -1;
    }

    for(n=0;n<5;n++)
    {
        if(gl_Recv_Signal.proto==14)
        {
             if(((GROUP_INFO_DATA *)gl_Recv_Signal.data)->flag==1)
             {
                 memset(&gl_Recv_Signal,NULL,sizeof(gl_Recv_Signal));
                 return 0;
             }
             else if(((GROUP_INFO_DATA *)gl_Recv_Signal.data)->flag==-1)
             {
                 memset(&gl_Recv_Signal,NULL,sizeof(gl_Recv_Signal));
                 return -2;
             }
             else
             {
                 memset(&gl_Recv_Signal,NULL,sizeof(gl_Recv_Signal));
                 return -3;
             }
        }
        Sleep(1000);
    }

    return -4;
}

int ServerConn::change_online_status(int status)
{
    FRIEND_INFO_DATA friendInfoData;

    memset(&friendInfoData,NULL,sizeof(friendInfoData));

    friendInfoData.onlineStatus=status;
    friendInfoData.friendInfo=accountInfo;

    ex_send((char *)&friendInfoData,sizeof(friendInfoData),6);

    return 0;
}

FileSendThread::FileSendThread(FILE_TRAN_LIST fileTranList)
{
    p2pMode=0;
    this->fileTranList=fileTranList;
    connect(this,SIGNAL(signal_update_send_info(int,int,char *)),glp_ChatWindow,SLOT(slot_update_send_info(int,int,char *)));
    return;
}

FileSendThread::FileSendThread(FILE_TRAN_LIST fileTranList, SOCKET soc,struct sockaddr_in addr)
{
    p2pMode=1;
    this->soc=soc;
    this->addr=addr;
    this->fileTranList=fileTranList;
    connect(this,SIGNAL(signal_update_send_info(int,int,char *)),glp_ChatWindow,SLOT(slot_update_send_info(int,int,char *)));
    return;
}

void FileSendThread::run()
{
    if(p2pMode)
        p2p_send_file_to_friend();
    else
        send_file_to_friend();
    return;
}

void FileSendThread::send_file_to_friend()
{
    FILE *pFile=NULL;
    int tranDoneSize=0;

    pFile=fopen(fileTranList.filePath,"rb");
    if(pFile==NULL)
    {
        qDebug() <<"fopen() error";
        return;
    }
    do
    {
        memset(fileTranList.fileTranData.fileData,NULL,sizeof(fileTranList.fileTranData.fileData));
        fileTranList.fileTranData.dataSize=fread(fileTranList.fileTranData.fileData,1,\
                                                 sizeof(fileTranList.fileTranData.fileData),pFile);
        if(feof(pFile))
            fileTranList.fileTranData.continueFlag=0;
        else
            fileTranList.fileTranData.continueFlag=1;
        fileTranList.fileTranData.auth=2;
        gl_Server_Conn_Info.ex_send((char *)&fileTranList.fileTranData,sizeof(FILE_TRAN_DATA),10);
        tranDoneSize+=fileTranList.fileTranData.dataSize;
        signal_update_send_info(tranDoneSize,fileTranList.fileTranData.fileSize,fileTranList.fileTranData.toNum);
    }while(fileTranList.fileTranData.continueFlag);
    fclose(pFile);

    return;
}

void FileSendThread::p2p_send_file_to_friend()
{
    int i;
    vector<FILE_TRAN_LIST>::iterator iterFileSendList;
    FILE *pFile=NULL;
    int tranDoneSize=0;
    P2P_PACKET packet;
    P2P_TRAN_FILE *pP2PTranFile=NULL;
    P2P_TRAN_INFO_DATA *tranData=NULL;

    pFile=fopen(fileTranList.filePath,"rb");
    if(pFile==NULL)
    {
        qDebug() <<"fopen() error";
        signal_update_send_info(0,0,fileTranList.fileTranData.toNum);
        return;
    }
    for(iterFileSendList=gl_Server_Conn_Info.vectorFileTranList.begin();\
        iterFileSendList!=gl_Server_Conn_Info.vectorFileTranList.end();iterFileSendList++)
    {
        if((*iterFileSendList).fileTranData.sequenceNum==fileTranList.fileTranData.sequenceNum)
        {
            tranData=&(*iterFileSendList).tranData;
            break;
        }
    }
    qDebug() <<"开始发送文件.";
    i=fileTranList.fileTranData.fileSize/100;
    do
    {
        memset(&packet,NULL,sizeof(packet));
        pP2PTranFile=(P2P_TRAN_FILE *)packet.data;
        packet.proto=2;
        pP2PTranFile->dataSize=fread(pP2PTranFile->fileData,1,\
                                                 sizeof(pP2PTranFile->fileData),pFile);
        if(feof(pFile))
            pP2PTranFile->continueFlag=0;
        else
            pP2PTranFile->continueFlag=1;

        if(gl_P2P.p2p_send(this->soc,tranData,(char *)&packet,sizeof(packet),&this->addr)<=2)
        {
            qDebug() <<"P2P 连接断开";
            break;
        }
        tranDoneSize+=pP2PTranFile->dataSize;
        if(i-tranDoneSize<=0)
        {
            i=tranDoneSize+fileTranList.fileTranData.fileSize/100;
            signal_update_send_info(tranDoneSize,fileTranList.fileTranData.fileSize,fileTranList.fileTranData.toNum);
        }
    }while(pP2PTranFile->continueFlag);
    closesocket(this->soc);
    fclose(pFile);
    signal_update_send_info(fileTranList.fileTranData.fileSize,fileTranList.fileTranData.fileSize,fileTranList.fileTranData.toNum);

    return;
}

P2PFileRecvThread::P2PFileRecvThread(FILE_TRAN_DATA fileTranData)
{
    this->fileTranData=fileTranData;
    connect(this,SIGNAL(signal_update_recv_info(int,int,char *)),glp_ChatWindow,SLOT(slot_update_recv_info(int,int,char *)));
    return;
}

void P2PFileRecvThread::run()
{
    int n;
    SOCKET soc;
    P2P_CONN_LIST *pP2PConnList=NULL;

    for(n=0;n<20;n++)
    {
        if(gl_P2P.p2p_client_list_search(fileTranData.toNum,&pP2PConnList)==0)
        {
            soc=pP2PConnList->soc;
            gl_P2P.p2p_client_list_del_node(fileTranData.toNum);
            break;
        }
        Sleep(1000);
    }
    if(n>=20)
    {
        //connect timeout
        qDebug() <<"文件接收连接建立失败";
        disconnect(this,SIGNAL(signal_update_recv_info(int,int,char *)),glp_ChatWindow,SLOT(slot_update_recv_info(int,int,char *)));
        return;
    }
    qDebug() <<"文件接收连接建立成功";
    recv_file_from_friend(soc);
    disconnect(this,SIGNAL(signal_update_recv_info(int,int,char *)),glp_ChatWindow,SLOT(slot_update_recv_info(int,int,char *)));

    return;
}

void P2PFileRecvThread::recv_file_from_friend(SOCKET soc)
{
    vector<FILE_TRAN_LIST>::iterator iterFileRecvList;
    P2P_PACKET packet;
    P2P_TRAN_FILE *pP2PTranFile=NULL;
    struct sockaddr_in addr;
    int done=0;
    FILE *pFile=NULL;
    int i;
    P2P_TRAN_INFO_DATA *tranData=NULL;

    for(iterFileRecvList=gl_Server_Conn_Info.vectorFileRecvList.begin();\
        iterFileRecvList!=gl_Server_Conn_Info.vectorFileRecvList.end();iterFileRecvList++)
    {
        if((*iterFileRecvList).fileTranData.sequenceNum==this->fileTranData.sequenceNum)
        {
            pFile=(*iterFileRecvList).pFile;
            tranData=&(*iterFileRecvList).tranData;
            break;
        }
    }
    if(pFile==NULL)
    {
        qDebug() <<"File recv sequence not find";
        closesocket(soc);
        return;
    }
    qDebug() <<"开始接收文件.";
    i=fileTranData.fileSize/100;
    do
    {
        memset(&packet,NULL,sizeof(packet));
        memset(&addr,NULL,sizeof(addr));
        if(gl_P2P.p2p_recv(soc,tranData,(char *)&packet,sizeof(packet),&addr)<=0)
            break;
        if(packet.proto!=2)
            continue;
        pP2PTranFile=(P2P_TRAN_FILE *)packet.data;
        fwrite(pP2PTranFile->fileData,pP2PTranFile->dataSize,1,pFile);
        fflush(pFile);
        done+=pP2PTranFile->dataSize;
        if(i-done<=0)
        {
            i=done+fileTranData.fileSize/100;
            signal_update_recv_info(done,this->fileTranData.fileSize,this->fileTranData.toNum);
        }
    }while(pP2PTranFile->continueFlag);
    qDebug() <<"Recv finish";
    fclose(pFile);
    closesocket(soc);
    signal_update_recv_info(this->fileTranData.fileSize,this->fileTranData.fileSize,this->fileTranData.toNum);
    Sleep(1000);

    return;
}

KeepAliveThread::KeepAliveThread()
{
    return;
}

KeepAliveThread::func(void *threadPara)
{
    MAIN_PACKET packet;

    memset(&packet,0x00,sizeof(packet));

    while(1)
    {
        gl_Server_Conn_Info.ex_send((char *)&packet,sizeof(packet),0);
        Sleep(10000);   //十秒一次心跳包
    }
}













