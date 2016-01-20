#ifndef RECV_H_INCLUDED
#define RECV_H_INCLUDED

#include "../global.h"
#include "../p2p/p2p.h"

typedef struct
{
    int tranDoneSize;
    char filePath[1000];
    FILE *pFile;
    P2P_TRAN_INFO_DATA tranData;
    FILE_TRAN_DATA fileTranData;
}FILE_TRAN_LIST;

class ServerConn
{
public:
    ServerConn();
    ~ServerConn();
    int connect_to_server();
    int auth_account(char *num,char *password);
    int register_account(REGISTER_ACCOUNT_DATA *pRegData);
    int find_friend(ACCOUNT_INFO_DATA *pAccountInfo);
    int ex_send(char *sendBuffer,unsigned int bufferSize,int proto);
    int add_friend(FRIENDSHIP_INFO_DATA *pFriendshipInfo);
    int update_account_data(ACCOUNT_INFO_DATA *pAccountData);
    int upload_icon(char *pFilePath);
    int send_sms(SMS_DATA *pSMSData);
    int send_sms(GROUP_SMS_DATA *pGroupSMSData);
    char *current_time_string();
    int add_file_tran_list(FILE_TRAN_LIST fileTranList);
    int delete_friend(char *num);
    int create_group(CREATE_GROUP_DATA *pCreateGroupData);
    int find_group(GROUP_INFO_DATA *pGroupInfoData);
    int join_group(GROUP_INFO_DATA *pGroupInfoData);
    int change_online_status(int status);
public:
    ACCOUNT_INFO_DATA accountInfo;
    char ID[ID_MAXIMUM_SIZE];
    SOCKET soc;
    struct sockaddr_in addr;
    int recvSize;
    char recvBuffer[MAX_RECV_SIZE];
    char iconPath[1000];
    char tmpPath[1000];
    FILE *pFile;
    vector<FILE_TRAN_LIST> vectorFileTranList;
    vector<FILE_TRAN_LIST> vectorFileRecvList;
private:
    CRITICAL_SECTION CRI_Send;
};

class RecvThread : public QThread
{
    Q_OBJECT
public:
    explicit RecvThread(QObject *parent = 0);
    void run();
signals:
    void signal_update_friend_item(FRIEND_INFO_DATA FriendInfoData);
    void signal_update_item_icon(char *num);
    void signal_add_sms_unread(SMS_DATA smsData);
    void signal_add_sms_log(SMS_DATA smsData);
    void signal_add_sms_log(GROUP_SMS_DATA smsData);
    void signal_show_sms_on_text(SMS_DATA smsData);
    void signal_show_sms_on_text(GROUP_SMS_DATA smsData);
    void signal_recv_file(FILE_TRAN_DATA fileTranData);
    void signal_reject_recv_file(FILE_TRAN_DATA fileTranData);
    void signal_start_tran_file(FILE_TRAN_DATA fileTranData);
    void signal_update_recv_info(int done,int total,char *num);
    void signal_update_myself_icon();
    void signal_delete_friend(char *num);
    void signal_update_group_list_item(GROUP_INFO_DATA groupInfoData);
public slots:
private:
    int recv_data_from_server();
    int recv_icon(ICON_TRAN_DATA *pIconData);
};

class KeepAliveThread : public Thread
{
public:
    KeepAliveThread();
protected:
    int func(void *threadPara);
};

class FileSendThread : public QThread
{
    Q_OBJECT
public:
    FileSendThread(FILE_TRAN_LIST fileTranList);
    FileSendThread(FILE_TRAN_LIST fileTranList,SOCKET soc,struct sockaddr_in addr);
    void run();
private:
    int p2pMode;
    SOCKET soc;
    struct sockaddr_in addr;
    void send_file_to_friend();
    void p2p_send_file_to_friend();
public:
    FILE_TRAN_LIST fileTranList;
signals:
    signal_update_send_info(int done,int total,char *num);
};

class P2PFileRecvThread : public QThread
{
    Q_OBJECT
public:
    P2PFileRecvThread(FILE_TRAN_DATA fileTranData);
    void run();
private:
    void recv_file_from_friend(SOCKET soc);
    FILE_TRAN_DATA fileTranData;
signals:
    void signal_update_recv_info(int done,int total,char *num);
};

#endif // RECV_H_INCLUDED
