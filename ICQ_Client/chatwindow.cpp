#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "filesendform.h"
#include "global.h"
#include "chatform.h"
#include "commun/commun.h"
#include "friendstreeview.h"
#include "p2p/p2p.h"

ChatWindow *glp_ChatWindow;
extern ServerConn gl_Server_Conn_Info;
extern FriendTreeOperation *gl_FriendTree_Operation;
extern RecvThread *recvThread;
extern P2P gl_P2P;

ChatItemWidget::ChatItemWidget(FRIEND_INFO_DATA *pFriendInfoData)
{
    QPixmap *icon=NULL;
    char iconPath[500];

    memset(iconPath,NULL,sizeof(iconPath));
    strcat(iconPath,gl_Server_Conn_Info.tmpPath);
    strcat(iconPath,pFriendInfoData->friendInfo.num);
    strcat(iconPath,".jpg");

    labelIcon = new QLabel(this);
    labelName = new QLabel(this);
    labelName->setStyleSheet("QLabel{color: green; font: 11pt bold;}");

    verlayout = new QVBoxLayout();
    verlayout->setContentsMargins(0, 0, 0, 0);
    verlayout->addWidget(labelName);

    horLayout = new QHBoxLayout(this);
    horLayout->setContentsMargins(2, 2, 2, 2);
    horLayout->addWidget(labelIcon, 0, Qt::AlignTop);
    horLayout->addLayout(verlayout, 4);

    labelName->setText(pFriendInfoData->friendInfo.nickname);
    if(access(iconPath,0)!=0)
    {
        if(pFriendInfoData->friendInfo.sex==1)
        {
            icon=new QPixmap(":/images/portrait/res/boy.jpg");
            labelIcon->setPixmap(icon->scaled(43,40));
        }
        else
        {
            icon=new QPixmap(":/images/portrait/res/girl.jpg");
            labelIcon->setPixmap(icon->scaled(43,40));
        }
    }
    else
    {
        icon=new QPixmap(iconPath);
        labelIcon->setPixmap(icon->scaled(43,40));
    }

    pChatForm=new ChatForm();
    pChatForm->set_info(pFriendInfoData);
    setStyleSheet("background-color:rgba(138,43,226,30)");
    this->friendInfoData=*pFriendInfoData;
}

ChatItemWidget::ChatItemWidget(GROUP_INFO_DATA *pGroupInfoData)
{
    QPixmap *icon=NULL;

    labelIcon = new QLabel(this);
    labelName = new QLabel(this);
    labelName->setStyleSheet("QLabel{color: green; font: 11pt bold;}");

    verlayout = new QVBoxLayout();
    verlayout->setContentsMargins(0, 0, 0, 0);
    verlayout->addWidget(labelName);

    horLayout = new QHBoxLayout(this);
    horLayout->setContentsMargins(2, 2, 2, 2);
    horLayout->addWidget(labelIcon, 0, Qt::AlignTop);
    horLayout->addLayout(verlayout, 4);

    labelName->setText(pGroupInfoData->groupName);
    icon=new QPixmap(":/images/portrait/res/groupIcon.jpg");
    labelIcon->setPixmap(icon->scaled(43,40));
    this->groupInfoData=*pGroupInfoData;

    pChatForm=new ChatForm();
    pChatForm->set_info(pGroupInfoData);
    setStyleSheet("background-color:rgba(138,43,226,30)");
}

ChatWindow::ChatWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);    //将widget的标题栏和边框去掉
    ui->listWidget->setStyleSheet("background-color:rgba(255,255,255,0)");
    connect(ui->listWidget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,\
            SLOT(slot_change_chat_form(QListWidgetItem*,QListWidgetItem*)));
    qRegisterMetaType<GROUP_SMS_DATA>("GROUP_SMS_DATA");
    connect(recvThread,SIGNAL(signal_show_sms_on_text(SMS_DATA)),this,SLOT(slot_show_sms_on_text(SMS_DATA)));
    connect(recvThread,SIGNAL(signal_show_sms_on_text(GROUP_SMS_DATA)),this,SLOT(slot_show_sms_on_text(GROUP_SMS_DATA)));
    qRegisterMetaType<FILE_TRAN_DATA>("FILE_TRAN_DATA");
    connect(recvThread,SIGNAL(signal_recv_file(FILE_TRAN_DATA)),this,SLOT(slot_recv_file(FILE_TRAN_DATA)));
    connect(recvThread,SIGNAL(signal_reject_recv_file(FILE_TRAN_DATA)),this,SLOT(slot_reject_recv_file(FILE_TRAN_DATA)));
    connect(recvThread,SIGNAL(signal_start_tran_file(FILE_TRAN_DATA)),this,SLOT(slot_start_tran_file(FILE_TRAN_DATA)));
    connect(recvThread,SIGNAL(signal_update_recv_info(int,int,char *)),this,SLOT(slot_update_recv_info(int,int,char *)));

    return;
}

void ChatWindow::set_chat_item(FRIEND_INFO_DATA *pFriendInfoData)
{
    ChatItemWidget *pChatItemWidget=NULL;
    QListWidgetItem *pListItemWidget=NULL;
    vector<QListWidgetItem *>::iterator iterItem;

    for(iterItem=vectorListItem.begin();iterItem!=vectorListItem.end();iterItem++)
    {
        pChatItemWidget=(ChatItemWidget *)ui->listWidget->itemWidget(*iterItem);
        if(!strcmp(pFriendInfoData->friendInfo.num,pChatItemWidget->friendInfoData.friendInfo.num))
        {
            ui->listWidget->setCurrentItem(*iterItem);
            return;
        }
    }
    pChatItemWidget=new ChatItemWidget(pFriendInfoData);
    pListItemWidget=new QListWidgetItem(ui->listWidget);
    pListItemWidget->setSizeHint(QSize(200, 40));
    pChatItemWidget->friendInfoData=*pFriendInfoData;
    ui->listWidget->setItemWidget(pListItemWidget,pChatItemWidget);
    ui->listWidget->setCurrentItem(pListItemWidget);
    vectorListItem.push_back(pListItemWidget);

    return;
}

void ChatWindow::set_chat_item(GROUP_INFO_DATA *pGroupInfoData)
{
    ChatItemWidget *pChatItemWidget=NULL;
    QListWidgetItem *pListItemWidget=NULL;
    vector<QListWidgetItem *>::iterator iterItem;

    for(iterItem=vectorListItem.begin();iterItem!=vectorListItem.end();iterItem++)
    {
        pChatItemWidget=(ChatItemWidget *)ui->listWidget->itemWidget(*iterItem);
        if(!strcmp(pGroupInfoData->groupNum,pChatItemWidget->groupInfoData.groupNum))
        {
            ui->listWidget->setCurrentItem(*iterItem);
            return;
        }
    }
    pChatItemWidget=new ChatItemWidget(pGroupInfoData);
    pListItemWidget=new QListWidgetItem(ui->listWidget);
    pListItemWidget->setSizeHint(QSize(200, 40));
    pChatItemWidget->groupInfoData=*pGroupInfoData;
    ui->listWidget->setItemWidget(pListItemWidget,pChatItemWidget);
    ui->listWidget->setCurrentItem(pListItemWidget);
    vectorListItem.push_back(pListItemWidget);

    return;
}

void ChatWindow::remove_chat_item(ChatForm *pChatForm)
{
    ChatItemWidget *pChatItemWidget=NULL;
    vector<QListWidgetItem *>::iterator iterItem;

    for(iterItem=vectorListItem.begin();iterItem!=vectorListItem.end();iterItem++)
    {
        pChatItemWidget=(ChatItemWidget *)ui->listWidget->itemWidget(*iterItem);
        if(pChatItemWidget->pChatForm==pChatForm)
        {
            if(vectorListItem.size()>1)
            {
                if(iterItem!=vectorListItem.begin())
                    ui->listWidget->setCurrentItem(*(vectorListItem.begin()));
                else
                {
                    ui->listWidget->setCurrentItem(*(iterItem+1));
                }
            }
            pChatForm->close();
            ui->listWidget->removeItemWidget(*iterItem);
            delete *iterItem;
            vectorListItem.erase(iterItem);
            if(vectorListItem.empty())
            {
                close();
            }
            return;
        }
    }
}

void ChatWindow::clear_chat_item()
{
    ChatItemWidget *pChatItemWidget=NULL;
    vector<QListWidgetItem *>::iterator iterItem;

    for(iterItem=vectorListItem.begin();iterItem!=vectorListItem.end();)
    {
        pChatItemWidget=(ChatItemWidget *)ui->listWidget->itemWidget(*iterItem);
        pChatItemWidget->pChatForm->close();

        if(vectorListItem.size()>1)
        {
            if(iterItem!=vectorListItem.begin())
                ui->listWidget->setCurrentItem(*(vectorListItem.begin()));
            else
            {
                ui->listWidget->setCurrentItem(*(iterItem+1));
            }
        }
        ui->listWidget->removeItemWidget(*iterItem);

        delete pChatItemWidget->pChatForm;
        delete pChatItemWidget;
        delete *iterItem;
        vectorListItem.erase(iterItem);
        iterItem=vectorListItem.begin();
    }
    vectorListItem.clear();

    return;
}

ChatItemWidget *ChatWindow::search_chat_item_by_num(char *num)
{
    vector<QListWidgetItem *>::iterator iterItem;
    ChatItemWidget *pChatItemWidget=NULL;

    for(iterItem=vectorListItem.begin();iterItem!=vectorListItem.end();iterItem++)
    {
        pChatItemWidget=(ChatItemWidget *)ui->listWidget->itemWidget(*iterItem);
        if(!strcmp(pChatItemWidget->friendInfoData.friendInfo.num,num))
        {
            return pChatItemWidget;
        }
    }

    return NULL;
}

ChatItemWidget *ChatWindow::search_group_chat_item_by_num(char *num)
{
    vector<QListWidgetItem *>::iterator iterItem;
    ChatItemWidget *pChatItemWidget=NULL;

    for(iterItem=vectorListItem.begin();iterItem!=vectorListItem.end();iterItem++)
    {
        pChatItemWidget=(ChatItemWidget *)ui->listWidget->itemWidget(*iterItem);
        if(!strcmp(pChatItemWidget->groupInfoData.groupNum,num))
        {
            return pChatItemWidget;
        }
    }

    return NULL;
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(m_pixmapBg.rect(), m_pixmapBg);
    painter.drawPixmap(0,0,width(),height(),QPixmap(":/images/background/res/chatBG2.jpg"));   //加载背景图片
}

void ChatWindow::mouseMoveEvent(QMouseEvent *e)
{
    this->move(e->globalPos() - m_pointStart);
}

void ChatWindow::mousePressEvent(QMouseEvent *e)
{
    m_pointPress = e->globalPos();
    m_pointStart = m_pointPress - this->pos();

}

void ChatWindow::slot_change_chat_form(QListWidgetItem *current, QListWidgetItem *previous)
{
    ChatItemWidget *pChatItemWidget=NULL;

    if(current==NULL)
        return;
    if(previous!=NULL)
    {
        pChatItemWidget=(ChatItemWidget *)ui->listWidget->itemWidget(previous);
        pChatItemWidget->pChatForm->close();

    }
    pChatItemWidget=(ChatItemWidget *)ui->listWidget->itemWidget(current);
    ui->chatLayout->addWidget(pChatItemWidget->pChatForm);
    pChatItemWidget->pChatForm->show();

    return;
}

void ChatWindow::slot_show_sms_on_text(SMS_DATA smsData)
{
    ChatItemWidget *ChatItemWidget=NULL;

    ChatItemWidget=search_chat_item_by_num(smsData.fromNum);
    if(ChatItemWidget==NULL)
        return;
    ChatItemWidget->pChatForm->append_text(smsData);

    return;
}

void ChatWindow::slot_show_sms_on_text(GROUP_SMS_DATA smsData)
{
    ChatItemWidget *ChatItemWidget=NULL;

    ChatItemWidget=search_group_chat_item_by_num(smsData.toGroupNum);
    if(ChatItemWidget==NULL)
        return;
    ChatItemWidget->pChatForm->append_text(smsData);

    return;
}

void ChatWindow::slot_recv_file(FILE_TRAN_DATA fileTranData)
{
    ChatItemWidget *pChatItemWidget=NULL;
    FriendItemWidget *pFriendItemWidget=NULL;

    cout <<"slot_recv_file:"<<fileTranData.fromNum<<endl;
    pChatItemWidget=glp_ChatWindow->search_chat_item_by_num(fileTranData.fromNum);
    if(pChatItemWidget==NULL)
    {
        //对话窗口未打开
        pFriendItemWidget=(FriendItemWidget *)gl_FriendTree_Operation->get_item_widget_by_num(fileTranData.fromNum);
        gl_FriendTree_Operation->sms_unread_close_remind(pFriendItemWidget->friendInfoData.friendInfo.num);
        glp_ChatWindow->set_chat_item(&pFriendItemWidget->friendInfoData);
        glp_ChatWindow->show();
        pChatItemWidget=glp_ChatWindow->search_chat_item_by_num(fileTranData.fromNum);
        if(pChatItemWidget==NULL)
        {
            cout <<"pChatItemWidget is NULL"<<endl;
            return;
        }
    }
    else
    {
        //窗口已打开
    }
    pChatItemWidget->pChatForm->fileRecvForm->set_recv_file_info(fileTranData);
    pChatItemWidget->pChatForm->show_file_recv_widget();

    return;
}

void ChatWindow::slot_reject_recv_file(FILE_TRAN_DATA fileTranData)
{
    ChatItemWidget *pChatItemWidget=NULL;
    vector<FILE_TRAN_LIST>::iterator iterFileTranList;

    pChatItemWidget=(ChatItemWidget *)search_chat_item_by_num(fileTranData.fromNum);
    if(pChatItemWidget==NULL)
        return;
    pChatItemWidget->pChatForm->fileSendForm->set_label_text("传输被拒绝.");
    for(iterFileTranList=gl_Server_Conn_Info.vectorFileTranList.begin();\
        iterFileTranList!=gl_Server_Conn_Info.vectorFileTranList.end();iterFileTranList++)
    {
        if((*iterFileTranList).fileTranData.sequenceNum==fileTranData.sequenceNum)
        {
            cout <<"delete list"<<endl;
            gl_Server_Conn_Info.vectorFileTranList.erase(iterFileTranList);
            return;
        }
    }
}

void ChatWindow::slot_start_tran_file(FILE_TRAN_DATA fileTranData)
{
    int n;
    SOCKET soc;
    struct sockaddr_in addr;
    char str[ID_MAXIMUM_SIZE];
    FILE_TRAN_LIST fileTranList;
    vector<FILE_TRAN_LIST>::iterator iterFileTranList;
    FileSendThread *pFileSendThread=NULL;
    ChatItemWidget *pChatItemWidget=NULL;
    P2P_CREATE_CONN_THREAD_PARA *pP2PThreadPara=NULL;
    P2P_CONN_LIST *pP2PConnList=NULL;

    memset(&fileTranList,NULL,sizeof(fileTranList));

    memset(str,NULL,sizeof(str));
    strcat(str,fileTranData.fromNum);
    memset(fileTranData.fromNum,NULL,sizeof(fileTranData.fromNum));
    strcat(fileTranData.fromNum,fileTranData.toNum);
    memset(fileTranData.toNum,NULL,sizeof(fileTranData.toNum));
    strcat(fileTranData.toNum,str);

    fileTranList.fileTranData=fileTranData;
    for(iterFileTranList=gl_Server_Conn_Info.vectorFileTranList.begin();\
        iterFileTranList!=gl_Server_Conn_Info.vectorFileTranList.end();iterFileTranList++)
    {
        if((*iterFileTranList).fileTranData.sequenceNum==fileTranData.sequenceNum)
        {
            fileTranList=*iterFileTranList;
            break;
        }
    }

    pChatItemWidget=search_chat_item_by_num(fileTranData.toNum);
    if(pChatItemWidget==NULL) return;
    pChatItemWidget->pChatForm->fileSendForm->set_label_text("正在建立连接...");
    pP2PThreadPara=(P2P_CREATE_CONN_THREAD_PARA *)malloc(sizeof(P2P_CREATE_CONN_THREAD_PARA));
    if(pP2PThreadPara==NULL)
        return;
    memset(pP2PThreadPara,NULL,sizeof(P2P_CREATE_CONN_THREAD_PARA));
    strcat(pP2PThreadPara->P2PConnInfo.fromID,fileTranData.fromNum);
    strcat(pP2PThreadPara->P2PConnInfo.toID,fileTranData.toNum);
    pP2PThreadPara->P2PConnInfo.flag=1;
    //建立P2P连接
    gl_P2P.p2p_create_connection_thread(pP2PThreadPara);
    for(n=0;n<15;n++)
    {
        if(gl_P2P.p2p_client_list_search(fileTranData.toNum,&pP2PConnList)==0)
        {
            soc=pP2PConnList->soc;
            addr=pP2PConnList->addr;
            gl_P2P.p2p_client_list_del_node(fileTranData.toNum);
            break;
        }
        Sleep(1000);
    }
    if(n>=15)
    {
        pFileSendThread=new FileSendThread(fileTranList);
        pFileSendThread->start();
    }
    else
    {
        pChatItemWidget->pChatForm->fileSendForm->set_label_text("建立P2P连接成功.");
        pFileSendThread=new FileSendThread(fileTranList,soc,addr);
        pFileSendThread->start();
    }

    return;
}

void ChatWindow::slot_update_send_info(int done, int total,char *num)
{
    char str[1000];
    ChatItemWidget *pChatItemWidget=NULL;

    pChatItemWidget=glp_ChatWindow->search_chat_item_by_num(num);
    if(pChatItemWidget==NULL)
    {
        //对话窗口未打开
        return;
    }
    else
    {
        //窗口已打开
        memset(str,NULL,sizeof(str));
        sprintf(str,"已传输：%d/%d KB",done/1024,total/1024);
        pChatItemWidget->pChatForm->fileSendForm->set_label_text(str);
        pChatItemWidget->pChatForm->fileSendForm->set_progress_range(0,total);
        pChatItemWidget->pChatForm->fileSendForm->set_progress_value(done);
        if(done>=total)
        {
            pChatItemWidget->pChatForm->fileSendForm->set_label_text("文件传输已完成.");
        }
    }

    return;
}

void ChatWindow::slot_update_recv_info(int done, int total, char *num)
{
    char str[1000];
    ChatItemWidget *pChatItemWidget=NULL;

    //qDebug() <<"slot_update_recv_info";
    pChatItemWidget=glp_ChatWindow->search_chat_item_by_num(num);
    if(pChatItemWidget==NULL)
    {
        //对话窗口未打开
        return;
    }
    else
    {
        //窗口已打开
        memset(str,NULL,sizeof(str));
        sprintf(str,"已接收：%d/%d KB",done/1024,total/1024);
        pChatItemWidget->pChatForm->fileRecvForm->set_label_text(str);
        pChatItemWidget->pChatForm->fileRecvForm->set_progress_range(0,total);
        pChatItemWidget->pChatForm->fileRecvForm->set_progress_value(done);
        if(done>=total)
        {
            pChatItemWidget->pChatForm->fileRecvForm->set_label_text("文件接收已完成.");
        }
    }

    return;
}










