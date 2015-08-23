#include "chatform.h"
#include "ui_chatform.h"
#include "chatwindow.h"
#include "commun/commun.h"
#include "friendstreeview.h"
#include "groupslistview.h"

extern ChatWindow *glp_ChatWindow;
extern RecvThread *recvThread;
extern ServerConn gl_Server_Conn_Info;
extern FriendTreeOperation *gl_FriendTree_Operation;
extern GroupListOperation *glp_GroupListOperation;

ChatForm::ChatForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatForm)
{
    ui->setupUi(this);

    fileSendForm=new FileSendForm();
    fileRecvForm=new FileRecvForm();

    ui->closeAllButton->setStyleSheet("QPushButton{border-image: url(:/images/button/res/close.png);}"
                                   "QPushButton:hover{border-image: url(:/images/button/res/closeP.png);}"
                                   "QPushButton:pressed{border-image: url(:/images/button/res/closeS.png);}");

    ui->minButton->setStyleSheet("QPushButton{border-image: url(:/images/button/res/narrow.png);}"
                                   "QPushButton:hover{border-image: url(:/images/button/res/narrowP.png);}"
                                   "QPushButton:pressed{border-image: url(:/images/button/res/narrowS.png);}");
    ui->sendFileButton->setStyleSheet("QPushButton{border-image: url(:/images/button/res/sendFile.jpg);}");
    ui->sendButton->setStyleSheet("QPushButton{background-color:rgba(255,255,255,100);}");
    ui->closeCurrentButton->setStyleSheet("QPushButton{background-color:rgba(255,255,255,100);}");
    setStyleSheet("background-color:rgba(255,255,255,0)");
}

void ChatForm::set_info(FRIEND_INFO_DATA *pFriendInfoData)
{
    int n;
    QString qstr;
    SMS_UNREAD smsUnread;
    SMS_LOG smsLog;

    ui->nameLabel->setText(pFriendInfoData->friendInfo.nickname);
    ui->signatureLabel->setText(pFriendInfoData->friendInfo.signature);

    qstr.sprintf("账号：%s",pFriendInfoData->friendInfo.num);
    ui->numLabel->setText(qstr);

    qstr.sprintf("姓名：%s",pFriendInfoData->friendInfo.truename);
    ui->turenameLabel->setText(qstr);

    qstr.sprintf("性别：%s",pFriendInfoData->friendInfo.sex==1?"男":"女");
    ui->sexLabel->setText(qstr);

    qstr.sprintf("年龄：%d",pFriendInfoData->friendInfo.age);
    ui->ageLabel->setText(qstr);

    qstr.sprintf("学校：%s",pFriendInfoData->friendInfo.school);
    ui->schoolLabel->setText(qstr);

    for(n=0;gl_FriendTree_Operation->fetch_sms_log_by_num(pFriendInfoData->friendInfo.num,&smsLog,&n)==0;)
    {
        ui->textBrowser->setTextColor(Qt::blue);
        ui->textBrowser->append(smsLog.head);
        ui->textBrowser->setTextColor(Qt::black);
        ui->textBrowser->append(smsLog.sms);
    }

    while(gl_FriendTree_Operation->fetch_sms_unread_by_num(pFriendInfoData->friendInfo.num,&smsUnread)==0)
    {
        ui->textBrowser->setTextColor(Qt::blue);
        ui->textBrowser->append(smsUnread.head);
        ui->textBrowser->setTextColor(Qt::black);
        ui->textBrowser->append(smsUnread.sms);
        gl_FriendTree_Operation->add_sms_log_by_num(pFriendInfoData->friendInfo.num,smsUnread);
    }
}

void ChatForm::set_info(GROUP_INFO_DATA *pGroupInfoData)
{
    int n;
    QString qstr;
    SMS_LOG smsLog;

    ui->friendInfoBox->setTitle("群信息");
    ui->nameLabel->setText(pGroupInfoData->groupName);
    ui->signatureLabel->setText(pGroupInfoData->groupIntro);

    qstr.sprintf("群号：%s",pGroupInfoData->groupNum);
    ui->numLabel->setText(qstr);

    qstr.sprintf("群名称：%s",pGroupInfoData->groupName);
    ui->turenameLabel->setText(qstr);

    ui->sexLabel->setText("");
    ui->ageLabel->setText("");
    ui->schoolLabel->setText("");
    ui->sendFileButton->close();

    for(n=0;glp_GroupListOperation->fetch_sms_log_by_num(pGroupInfoData->groupNum,&smsLog,&n)==0;)
    {
        ui->textBrowser->setTextColor(Qt::blue);
        ui->textBrowser->append(smsLog.head);
        ui->textBrowser->setTextColor(Qt::black);
        ui->textBrowser->append(smsLog.sms);
    }

    return;
}

void ChatForm::paintEvent(QPaintEvent *)
{
}

void ChatForm::mouseMoveEvent(QMouseEvent *e)
{
    glp_ChatWindow->move(e->globalPos() - m_pointStart);
}

void ChatForm::mousePressEvent(QMouseEvent *e)
{
    m_pointPress = e->globalPos();
    m_pointStart = m_pointPress - glp_ChatWindow->pos();

}

void ChatForm::show_file_send_widget()
{
    ui->fileTranLayout->addWidget(fileSendForm);
    fileSendForm->show();
}

void ChatForm::close_file_send_widget()
{
    fileSendForm->close();
}

void ChatForm::show_file_recv_widget()
{
    ui->fileTranLayout->addWidget(fileRecvForm);
    fileRecvForm->ex_show();
}

void ChatForm::close_file_recv_widget()
{
    fileRecvForm->close();
}

void ChatForm::append_text(SMS_DATA smsData)
{
    FriendItemWidget *pFriendItemWidget=NULL;
    QString qstr;
    char name[100];

    memset(name,NULL,sizeof(name));
    if(!strcmp(gl_Server_Conn_Info.accountInfo.num,smsData.fromNum))
    {
        strcat(name,"我");
    }
    else
    {
        pFriendItemWidget=gl_FriendTree_Operation->get_item_widget_by_num(smsData.fromNum);
        if(pFriendItemWidget==NULL) return;
        strcat(name,pFriendItemWidget->friendInfoData.friendInfo.nickname);
    }
    qstr.sprintf("[%s] %s",name,gl_Server_Conn_Info.current_time_string());
    ui->textBrowser->setTextColor(Qt::blue);
    ui->textBrowser->append(qstr);
    ui->textBrowser->setTextColor(Qt::black);
    ui->textBrowser->append(smsData.sms);

    return;
}

void ChatForm::append_text(GROUP_SMS_DATA groupSMSData)
{
    QString qstr;
    char name[100];

    memset(name,NULL,sizeof(name));
    if(!strcmp(gl_Server_Conn_Info.accountInfo.num,groupSMSData.fromFriendInfo.num))
    {
        strcat(name,"我");
    }
    else
    {
        strcat(name,groupSMSData.fromFriendInfo.nickname);
    }
    qstr.sprintf("[%s] %s",name,gl_Server_Conn_Info.current_time_string());
    ui->textBrowser->setTextColor(Qt::blue);
    ui->textBrowser->append(qstr);
    ui->textBrowser->setTextColor(Qt::black);
    ui->textBrowser->append(groupSMSData.sms);

    return;
}

ChatForm::~ChatForm()
{
    delete ui;
}

void ChatForm::on_minButton_clicked()
{
    glp_ChatWindow->showMinimized();
}

void ChatForm::on_closeAllButton_clicked()
{
    glp_ChatWindow->clear_chat_item();
    glp_ChatWindow->close();
}

void ChatForm::on_closeCurrentButton_clicked()
{
    glp_ChatWindow->remove_chat_item(this);
}

void ChatForm::on_sendButton_clicked()
{
    QString qstr;
    char *chStr=NULL,*p=NULL;
    SMS_DATA smsData;
    GROUP_SMS_DATA groupSMSData;

    memset(&smsData,NULL,sizeof(smsData));
    memset(&groupSMSData,NULL,sizeof(groupSMSData));

    ui->statusLabel->setText("");
    ui->statusLabel->repaint();
    qstr=ui->textEdit->toPlainText();
    if(qstr.isEmpty())
    {
        cout <<"input empty."<<endl;
        ui->statusLabel->setText("请输入内容.");
        return;
    }
    if(qstr.size()>=2000)
    {
        cout <<"sms length overstep."<<endl;
        ui->statusLabel->setText("超出长度.");
        return;
    }
    strcat(smsData.sms,QString_to_char(qstr));
    strcat(groupSMSData.sms,QString_to_char(qstr));

    qstr=ui->numLabel->text();
    chStr=QString_to_char(qstr);
    p=strchr(chStr,'：');
    if(p==NULL)
        return;
    p++;
    if(strstr(chStr,"账号"))
    {
        strcat(smsData.toNum,p);
        strcat(smsData.fromNum,gl_Server_Conn_Info.accountInfo.num);

        append_text(smsData);
        gl_FriendTree_Operation->slot_add_sms_log(smsData);
        if(gl_Server_Conn_Info.send_sms(&smsData)==-1)
        {
            //网络连接错误
            ui->statusLabel->setText("网络连接错误！");
            return;
        }
        ui->textEdit->setText("");
     }
    else
    {
        strcat(groupSMSData.toGroupNum,p);
        groupSMSData.fromFriendInfo=gl_Server_Conn_Info.accountInfo;

        if(gl_Server_Conn_Info.send_sms(&groupSMSData)==-1)
        {
            //网络连接错误
            ui->statusLabel->setText("网络连接错误！");
            return;
        }
        ui->textEdit->setText("");
    }

    return;
}

void ChatForm::on_sendFileButton_clicked()
{
    QString qstr;
    FILE_TRAN_LIST fileTranList;
    char *pFilePath=NULL,*p=NULL;
    FriendItemWidget *pFriendItemWidget=NULL;

    memset(&fileTranList,NULL,sizeof(fileTranList));

    qstr=ui->numLabel->text();
    p=QString_to_char(qstr);
    p=strchr(p,'：');
    p++;
    strcat(fileTranList.fileTranData.toNum,p);
    pFriendItemWidget=(FriendItemWidget *)gl_FriendTree_Operation->get_item_widget_by_num(p);
    if(pFriendItemWidget->friendInfoData.onlineStatus==0)
        return;
    qstr=QFileDialog::getOpenFileName(this,"选择文件",".","*.*");
    if(qstr.isEmpty())
    {
        qDebug() <<"file path is empty."<<endl;
        return;
    }

    pFilePath=QString_to_char(qstr);
    for(p=pFilePath+strlen(pFilePath)-1;*p!='/';p--);
    strcat(fileTranList.fileTranData.fileName,++p);
    qDebug() <<p<<endl;

    pFilePath=UTF8ToANSI(pFilePath);
    strcat(fileTranList.filePath,pFilePath);
    switch(gl_Server_Conn_Info.add_file_tran_list(fileTranList))
    {
    case 0:
        fileSendForm->set_label_text("等待好友接收文件...");
        fileSendForm->set_progress_range(0,1);
        fileSendForm->set_progress_value(0);
        show_file_send_widget();
        break;
    case -1:
        fileSendForm->set_label_text("打开文件失败！");
        fileSendForm->set_progress_range(0,1);
        fileSendForm->set_progress_value(0);
        show_file_send_widget();
    case -2:
        fileSendForm->set_label_text("网络连接错误！");
        fileSendForm->set_progress_range(0,1);
        fileSendForm->set_progress_value(0);
        show_file_send_widget();
    default:
        break;
    }

    return;
}












