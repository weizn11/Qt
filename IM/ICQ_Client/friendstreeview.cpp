#include "friendstreeview.h"
#include "commun/commun.h"
#include "chatwindow.h"
#include "ctooltip.h"

vector<FRIEND_INFO_DATA> gl_Friend_Vector;
FriendTreeOperation *gl_FriendTree_Operation;
extern RecvThread *recvThread;
extern ServerConn gl_Server_Conn_Info;
extern ChatWindow *glp_ChatWindow;
extern CToolTip *glp_CToolTip;

FriendItemWidget::FriendItemWidget()
{
    labelIcon = new QLabel(this);
    labelName = new QLabel(this);
    labelName->setStyleSheet("QLabel{color: green; font: 11pt bold;}");
    labelInfo = new QLabel(this);
    labelInfo->setStyleSheet("QLabel{color: gray;}");

    verlayout = new QVBoxLayout();
    verlayout->setContentsMargins(0, 0, 0, 0);
    verlayout->addWidget(labelName);
    verlayout->addWidget(labelInfo);

    horLayout = new QHBoxLayout(this);
    horLayout->setContentsMargins(2, 2, 2, 2);
    horLayout->addWidget(labelIcon, 1, Qt::AlignTop);
    horLayout->addLayout(verlayout, 4);
}

FriendItemWidget::FriendItemWidget(FRIEND_INFO_DATA *pFriendInfoData)
{
    labelIcon = new QLabel(this);
    labelName = new QLabel(this);
    labelName->setStyleSheet("QLabel{color: green; font: 11pt bold;}");
    labelInfo = new QLabel(this);
    labelInfo->setStyleSheet("QLabel{color: gray;}");

    verlayout = new QVBoxLayout();
    verlayout->setContentsMargins(0, 0, 0, 0);
    verlayout->addWidget(labelName);
    verlayout->addWidget(labelInfo);

    horLayout = new QHBoxLayout(this);
    horLayout->setContentsMargins(2, 2, 2, 2);
    horLayout->addWidget(labelIcon, 0, Qt::AlignTop);
    horLayout->addLayout(verlayout, 4);

    setItem(pFriendInfoData);
}

QPixmap FriendItemWidget::convertToGray(QImage image)
{
    int w,h;

    w = image.width();
    h = image.height();
    QImage iGray(w,h,QImage::Format_RGB32);
    for(int i=0; i<w; i++)
    {
        for(int j=0; j<h; j++)
        {
            QRgb pixel = image.pixel(i,j);
            int gray = qGray(pixel);
            QRgb grayPixel = qRgb(gray,gray,gray);
            iGray.setPixel(i,j,grayPixel);
        }
    }
    return QPixmap::fromImage(iGray);
}



int FriendItemWidget::setItem(FRIEND_INFO_DATA *pFriendInfoData)
{
    QPixmap icon;
    QImage *imageIcon=NULL;
    friendInfoData=*pFriendInfoData;
    char strInfo[34],iconPath[255];
    QString qstr;

    memset(iconPath,NULL,sizeof(iconPath));
    strcat(iconPath,gl_Server_Conn_Info.tmpPath);
    strcat(iconPath,pFriendInfoData->friendInfo.num);
    strcat(iconPath,".jpg");

    if(access(iconPath,0)!=0)
    {
        if(pFriendInfoData->friendInfo.sex==1)
        {
            imageIcon=new QImage(QString(":/images/portrait/res/boy.jpg"));
            if(pFriendInfoData->onlineStatus==0)
            {
                //离线需置灰头像
                icon=convertToGray(*imageIcon);
            }
            else
            {
                //在线状态
                icon=QPixmap::fromImage(*imageIcon);
            }
            labelIcon->setPixmap(icon.scaled(43,40));
        }
        else
        {
            imageIcon=new QImage(QString(":/images/portrait/res/girl.jpg"));
            if(pFriendInfoData->onlineStatus==0)
            {
                //离线需至灰头像
                cout <<"convertToGray"<<endl;
                icon=convertToGray(*imageIcon);
            }
            else
            {
                //在线状态
                icon=QPixmap::fromImage(*imageIcon);
            }
            labelIcon->setPixmap(icon.scaled(43,40));
        }
    }
    else
    {
        imageIcon=new QImage(QString(iconPath));
        if(pFriendInfoData->onlineStatus==0)
        {
            //离线需置灰头像
            icon=convertToGray(*imageIcon);
        }
        else
        {
            //在线状态
            icon=QPixmap::fromImage(*imageIcon);
        }
        labelIcon->setPixmap(icon.scaled(43,40));
    }
    labelName->setText(pFriendInfoData->friendInfo.nickname);

    if(strlen(pFriendInfoData->friendInfo.signature)<1)
    {
        strcat(pFriendInfoData->friendInfo.signature,"这个家伙很懒，没有留任何消息。");
    }
    memset(strInfo,NULL,sizeof(strInfo));
    if(strlen(pFriendInfoData->friendInfo.signature)>sizeof(strInfo)-4)
    {
        memcpy(strInfo,pFriendInfoData->friendInfo.signature,sizeof(strInfo)-4);
        strcat(strInfo,"...");
    }
    else
    {
        strcat(strInfo,pFriendInfoData->friendInfo.signature);
    }
    if(pFriendInfoData->onlineStatus)
        labelInfo->setText(qstr.sprintf("[在线]%s",strInfo));
    else
        labelInfo->setText(qstr.sprintf("[离线]%s",strInfo));
}

char *FriendItemWidget::get_item_account()
{
    return friendInfoData.friendInfo.num;
}

int FriendItemWidget::change_labelName_color(char *color)
{
    QString qstr;

    qstr.sprintf("QLabel{color: %s; font: 11pt bold;}",color);
    labelName->setStyleSheet(qstr);
    cout <<"change_labelName_color"<<endl;

    return 0;
}

// 鼠标悬停的时候，显示当前用户简要信息
bool FriendItemWidget::event(QEvent *e)
{
    if (e->type() == QEvent::ToolTip)
    {
        glp_CToolTip->showMessage(friendInfoData,QCursor::pos());
        qDebug() << "tool tip show";

    }
    else if (e->type() == QEvent::Leave)
    {
        glp_CToolTip->close();
        qDebug() << "tool tip leave";
    }
    return QWidget::event(e);
}

FriendTreeOperation::FriendTreeOperation(QWidget *parent)
{
    treeWidget=(QTreeWidget *)parent;
    treeWidget->clear();
    treeWidget->setIndentation(5);  //设置子节点开头空行
    pFriendInfoCard=new OneselfInfoWindow();

    return;
}

void FriendTreeOperation::slot_update_friend_item(FRIEND_INFO_DATA FriendInfoData)
{
    cout <<"recv slot_update_friend_item"<<endl;
    update_friend_item(&FriendInfoData);
    return;
}

void FriendTreeOperation::slot_update_item_icon(char *num)
{
    FriendItemWidget *friendItemWidget=NULL;
    vector<QTreeWidgetItem *>::iterator iterItem;
    FRIEND_INFO_DATA *pFriendInfoData=NULL;

    cout <<"slot_update_item_icon,"<<num<<endl;
    for(iterItem=vectorFriendItem.begin(); iterItem!=vectorFriendItem.end(); iterItem++)
    {
        friendItemWidget=(FriendItemWidget *)treeWidget->itemWidget(*iterItem,0);
        if(!strcmp(friendItemWidget->get_item_account(),num))
        {
            //此帐号已存在于列表中，则更新账号信息
            cout <<"account already exist."<<endl;
            pFriendInfoData=&friendItemWidget->friendInfoData;
            friendItemWidget->setItem(pFriendInfoData);
            return;
        }
    }

    return;
}

void FriendTreeOperation::slot_double_clicked_item(QTreeWidgetItem *friendItem, int index)
{
    FriendItemWidget *friendItemWidget=NULL;
    vector<QTreeWidgetItem *>::iterator iterItem;

    for(iterItem=vectorFriendItem.begin();iterItem!=vectorFriendItem.end();iterItem++)
    {
        if(*iterItem==friendItem)
        {
            friendItemWidget=(FriendItemWidget *)treeWidget->itemWidget(friendItem,0);
            sms_unread_close_remind(friendItemWidget->friendInfoData.friendInfo.num);
            glp_ChatWindow->set_chat_item(&friendItemWidget->friendInfoData);
            glp_ChatWindow->show();
        }
    }

    return;
}

int FriendTreeOperation::update_friend_item(FRIEND_INFO_DATA *pFriendInfoData)
{
    QTreeWidgetItem *group=NULL;
    FriendItemWidget *friendItemWidget=NULL;
    QTreeWidgetItem *friendItem=NULL;
    vector<QTreeWidgetItem *>::iterator iterItem;

    if(strlen(pFriendInfoData->group)<1)
    {
        cout <<"convert group name"<<endl;
        strcat(pFriendInfoData->group,"我的好友");
    }
    for(iterItem=vectorFriendGroup.begin(); iterItem!=vectorFriendGroup.end(); iterItem++)
    {
        if(!strcmp(QString_to_char((*iterItem)->text(0)),pFriendInfoData->group))
        {
            cout <<"group already exist.\n";
            group=*iterItem;
            goto skip_1;
        }
    }
    group=new QTreeWidgetItem(treeWidget);
    group->setText(0,pFriendInfoData->group);
    vectorFriendGroup.push_back(group);

skip_1:
    for(iterItem=vectorFriendItem.begin(); iterItem!=vectorFriendItem.end(); iterItem++)
    {
        friendItemWidget=(FriendItemWidget *)treeWidget->itemWidget(*iterItem,0);
        if(!strcmp(friendItemWidget->get_item_account(),pFriendInfoData->friendInfo.num))
        {
            //此帐号已存在于列表中，则更新账号信息
            cout <<"account already exist."<<endl;
            friendItemWidget->setItem(pFriendInfoData);
            if(pFriendInfoData->onlineStatus==1)
            {
                cout <<"set top item"<<endl;
                //treeWidget->setCurrentIndex();
            }
            return 0;
        }
    }
    friendItemWidget=new FriendItemWidget(pFriendInfoData);
    friendItem=new QTreeWidgetItem(group);
    vectorFriendItem.push_back(friendItem);

    treeWidget->setItemWidget(friendItem,0,friendItemWidget);

    return 0;
}

FriendItemWidget *FriendTreeOperation::get_item_widget_by_num(char *num)
{
    FriendItemWidget *friendItemWidget=NULL;
    vector<QTreeWidgetItem *>::iterator itemIter;

    cout <<"get_item_widget_by_num"<<endl;
    for(itemIter=vectorFriendItem.begin();itemIter!=vectorFriendItem.end();itemIter++)
    {
        friendItemWidget=(FriendItemWidget *)treeWidget->itemWidget(*itemIter,0);
        if(!strcmp(friendItemWidget->friendInfoData.friendInfo.num,num))
        {
            //cout <<"get item pointer"<<endl;
            return friendItemWidget;
        }
    }

    return NULL;
}

int FriendTreeOperation::fetch_sms_unread_by_num(char *num,SMS_UNREAD *smsUnread)
{
    FriendItemWidget *pFriendItemWidget=NULL;

    memset(smsUnread,NULL,sizeof(SMS_UNREAD));

    pFriendItemWidget=get_item_widget_by_num(num);
    if(pFriendItemWidget==NULL)
    {
        cout <<"Friend item is not exist"<<endl;
        return -1;
    }
    if(pFriendItemWidget->vectorSMSUnread.empty())
    {
        cout <<"vectorSMSUnread is empty"<<endl;
        return -2;
    }
    cout <<"fetch_sms_unread_by_num"<<endl;
    *smsUnread=*pFriendItemWidget->vectorSMSUnread.begin();
    pFriendItemWidget->vectorSMSUnread.erase(pFriendItemWidget->vectorSMSUnread.begin());

    return 0;
}

int FriendTreeOperation::fetch_sms_log_by_num(char *num, SMS_LOG *smsLog,int *n)
{
    FriendItemWidget *pFriendItemWidget=NULL;

    memset(smsLog,NULL,sizeof(SMS_LOG));

    pFriendItemWidget=get_item_widget_by_num(num);
    if(pFriendItemWidget==NULL)
    {
        cout <<"Friend item is not exist"<<endl;
        return -1;
    }
    if(pFriendItemWidget->vectorSMSLog.empty())
    {
        cout <<"vectorSMSLog is empty"<<endl;
        return -2;
    }
    cout <<"fetch_sms_log_by_num"<<endl;
    if((pFriendItemWidget->vectorSMSLog.begin()+*n)!=pFriendItemWidget->vectorSMSLog.end())
        *smsLog=*(pFriendItemWidget->vectorSMSLog.begin()+*n);
    else
        return -2;
    (*n)++;

    return 0;
}

int FriendTreeOperation::sms_unread_remind(char *num)
{
    FriendItemWidget *pFriendItemWidget=NULL;

    pFriendItemWidget=get_item_widget_by_num(num);
    if(pFriendItemWidget==NULL)
        return -1;
    pFriendItemWidget->change_labelName_color("red");

    return 0;
}

int FriendTreeOperation::sms_unread_close_remind(char *num)
{
    FriendItemWidget *pFriendItemWidget=NULL;

    pFriendItemWidget=get_item_widget_by_num(num);
    if(pFriendItemWidget==NULL)
        return -1;
    pFriendItemWidget->change_labelName_color("green");

    return 0;
}

void FriendTreeOperation::add_sms_log_by_num(char *num,SMS_UNREAD smsLog)
{
    vector<QTreeWidgetItem *>::iterator iterItem;
    FriendItemWidget *pFriendItemWidget=NULL;


    for(iterItem=vectorFriendItem.begin();iterItem!=vectorFriendItem.end();iterItem++)
    {
        pFriendItemWidget=(FriendItemWidget *)treeWidget->itemWidget(*iterItem,0);
        if(!strcmp(num,pFriendItemWidget->friendInfoData.friendInfo.num))
        {
            pFriendItemWidget->vectorSMSLog.push_back(smsLog);
            cout <<"slot_add_sms_log"<<endl;
            return;
        }
    }
    return;
}

int FriendTreeOperation::delete_friend(char *num)
{
    FriendItemWidget *pFriendItemWidget=NULL;
    vector<QTreeWidgetItem *>::iterator iterItem;

    pFriendItemWidget=get_item_widget_by_num(num);
    if(pFriendItemWidget==NULL) return -1;

    if(gl_Server_Conn_Info.delete_friend(num)==0)
    {
        //删除好友成功
        for(iterItem=vectorFriendItem.begin();iterItem!=vectorFriendItem.end();iterItem++)
        {
            if(pFriendItemWidget==(FriendItemWidget *)treeWidget->itemWidget(*iterItem,0))
            {
                treeWidget->removeItemWidget(*iterItem,0);
                delete *iterItem;
                vectorFriendItem.erase(iterItem);
                break;
            }
        }
    }

    return 0;
}

void FriendTreeOperation::slot_add_sms_unread(SMS_DATA smsData)
{
    vector<QTreeWidgetItem *>::iterator iterItem;
    FriendItemWidget *pFriendItemWidget=NULL;
    SMS_UNREAD smsUnread;

    for(iterItem=vectorFriendItem.begin();iterItem!=vectorFriendItem.end();iterItem++)
    {
        pFriendItemWidget=(FriendItemWidget *)treeWidget->itemWidget(*iterItem,0);
        if(!strcmp(smsData.fromNum,pFriendItemWidget->friendInfoData.friendInfo.num))
        {
            memset(&smsUnread,NULL,sizeof(smsUnread));
            strcat(smsUnread.sms,smsData.sms);
            sprintf(smsUnread.head,"[%s] %s",get_item_widget_by_num(smsData.fromNum)->friendInfoData.friendInfo.nickname,\
                    gl_Server_Conn_Info.current_time_string());
            pFriendItemWidget->vectorSMSUnread.push_back(smsUnread);
            sms_unread_remind(smsData.fromNum);
            cout <<"slot_add_sms_unread"<<endl;
            return;
        }
    }
    return;
}

void FriendTreeOperation::slot_add_sms_log(SMS_DATA smsData)
{
    vector<QTreeWidgetItem *>::iterator iterItem;
    FriendItemWidget *pFriendItemWidget=NULL;
    SMS_LOG smsLog;

    for(iterItem=vectorFriendItem.begin();iterItem!=vectorFriendItem.end();iterItem++)
    {
        pFriendItemWidget=(FriendItemWidget *)treeWidget->itemWidget(*iterItem,0);
        if(!strcmp(smsData.fromNum,pFriendItemWidget->friendInfoData.friendInfo.num))
        {
            memset(&smsLog,NULL,sizeof(smsLog));
            strcat(smsLog.sms,smsData.sms);
            sprintf(smsLog.head,"[%s] %s",get_item_widget_by_num(smsData.fromNum)->friendInfoData.friendInfo.nickname,\
                    gl_Server_Conn_Info.current_time_string());
            pFriendItemWidget->vectorSMSLog.push_back(smsLog);
            cout <<"slot_add_sms_log"<<endl;
            return;
        }
        else if(!strcmp(smsData.toNum,pFriendItemWidget->friendInfoData.friendInfo.num))
        {
            memset(&smsLog,NULL,sizeof(smsLog));
            strcat(smsLog.sms,smsData.sms);
            sprintf(smsLog.head,"[我] %s",gl_Server_Conn_Info.current_time_string());
            pFriendItemWidget->vectorSMSLog.push_back(smsLog);
            cout <<"slot_add_sms_log"<<endl;
            return;
        }
    }
    return;
}

void FriendTreeOperation::slot_delete_friend(char *num)
{
    this->delete_friend(num);
    return;
}













