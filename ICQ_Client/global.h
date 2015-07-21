#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <time.h>
#include <winsock2.h>
#include <vector>
#include <QApplication>
#include <QDialog>
#include <QtGui>
#include <QtCore>
#include <QMessageBox>
#include <QEvent>
#include <QCursor>
#include <QWidget>
#include <QThread>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QTreeWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QGroupBox>

#include "basic/encode.h"

using namespace std;

#define SERVER_IP "192.168.8.128"
#define SERVER_PORT 7437

#define MAX_RECV_SIZE 3000
#define PROTO_DATA_MAX_SIZE 2048
#define ID_MAXIMUM_SIZE 20
#define PASSWORD_MAXIMUM_SIZE 30

typedef struct
{
    int success;
    char num[ID_MAXIMUM_SIZE];
    char nickname[100];
    char truename[100];
    char school[100];
    char password[30];
    char signature[140];
    char groupList[500];
    int sex;
    int age;
}REGISTER_ACCOUNT_DATA,ACCOUNT_INFO_DATA;

typedef struct
{
    int success;
    char num1[ID_MAXIMUM_SIZE];
    char num2[ID_MAXIMUM_SIZE];
    char group1[50];
    char group2[50];
}FRIENDSHIP_INFO_DATA;

typedef struct
{
    int onlineStatus;    //1:online     0:offline
    ACCOUNT_INFO_DATA friendInfo;
    char group[50];
}FRIEND_INFO_DATA;

typedef struct
{
    int flushFlag;
    int flag;   //1:continue    0:finish
    int dataSize;
    char num[ID_MAXIMUM_SIZE];
    char data[1024];
}ICON_TRAN_DATA;

typedef struct
{
    char fromNum[ID_MAXIMUM_SIZE];
    char toNum[ID_MAXIMUM_SIZE];
    char sms[2000];
}SMS_DATA;

typedef struct
{
    unsigned int sequenceNum;   //序列号
    int auth;           //1:通过许可    0:请求传输  -1:传输拒绝  2:文件流
    int continueFlag;      //1:continue    0:finish
    unsigned long fileSize;
    int dataSize;
    char fromNum[ID_MAXIMUM_SIZE];
    char toNum[ID_MAXIMUM_SIZE];
    char fileName[255];
    char fileData[1500];
}FILE_TRAN_DATA;

typedef struct
{
    int flag;
    char groupNum[ID_MAXIMUM_SIZE];
    char groupName[100];
    char groupIntro[200];
}CREATE_GROUP_DATA,GROUP_INFO_DATA;

typedef struct
{
    char toGroupNum[ID_MAXIMUM_SIZE];
    ACCOUNT_INFO_DATA fromFriendInfo;
    char sms[1000];
}GROUP_SMS_DATA;

typedef struct
{
    int user;       //1:friend  0:me
    char head[100];
    char sms[2048];
}SMS_LOG,SMS_UNREAD;

typedef struct
{
    /*
     * 1:P2P_CONN_INFO 2:AUTH_ACCOUNT_DATA 3:REGISTER_ACCOUNT_DATA 4:find_find(),ACCOUNT_INFO_DATA
     * 5:add_friend() FRIENDSHIP_INFO_DATA  6:update_friend_item FRIEND_INFO_DATA
     * 7:update_account_data() ACCOUNT_INFO_DATA  8:ICON_TRAN_DATA
     * 9:SMS_DATA   10:FILE_TRAN_DATA  11:delete_friend() FRIENDSHIP_INFO_DATA
     * 12:CREATE_GROUP_DATA  13:find_group() GROUP_INFO_DATA  14:join_group() GROUP_INFO_DATA
     * 15:update_groups_item GROUP_INFO_DATA   16:GROUP_SMS_DATA
     */
    unsigned int proto;    //上层协议类型
    char data[PROTO_DATA_MAX_SIZE];     //协议数据
}MAIN_PACKET,RECV_SIGNAL;

#endif // GLOBAL_H_INCLUDED
