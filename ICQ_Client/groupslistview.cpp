#include "groupslistview.h"
#include "chatwindow.h"
#include "commun/commun.h"

GroupListOperation *glp_GroupListOperation;
extern ChatWindow *glp_ChatWindow;
extern ServerConn gl_Server_Conn_Info;

GroupItemWidget::GroupItemWidget(GROUP_INFO_DATA groupInfoData)
{
    QPixmap *icon=NULL;

    this->groupInfoData=groupInfoData;
    labelIcon=new QLabel(this);
    labelName=new QLabel(this);
    horLayout=new QHBoxLayout(this);

    labelName->setStyleSheet("QLabel{color: green; font: 11pt bold;}");
    labelName->setText(groupInfoData.groupName);
    icon=new QPixmap(":/images/portrait/res/groupIcon.jpg");
    labelIcon->setPixmap(icon->scaled(43,40));

    horLayout->setContentsMargins(2, 2, 2, 2);
    horLayout->addWidget(labelIcon);
    horLayout->addWidget(labelName,1);

    return;
}

GroupListOperation::GroupListOperation(QWidget *parent)
{
    this->listWidget=(QListWidget *)parent;
    return;
}

GroupItemWidget *GroupListOperation::get_item_widget_by_num(char *num)
{
    GroupItemWidget *pGroupItemWidget=NULL;
    vector<QListWidgetItem *>::iterator iterItem;

    for(iterItem=vectorListWidgetItem.begin();iterItem!=vectorListWidgetItem.end();iterItem++)
    {
        pGroupItemWidget=(GroupItemWidget *)listWidget->itemWidget(*iterItem);
        if(!strcmp(pGroupItemWidget->groupInfoData.groupNum,num))
            return pGroupItemWidget;
    }

    return NULL;
}

int GroupListOperation::update_item(GROUP_INFO_DATA groupInfoData)
{
    QListWidgetItem *pListItem=NULL;
    GroupItemWidget *pGroupItemWidget=NULL;

    if(get_item_widget_by_num(groupInfoData.groupNum)!=NULL)
        return -1;
    pGroupItemWidget=new GroupItemWidget(groupInfoData);
    pListItem=new QListWidgetItem(listWidget);
    pListItem->setSizeHint(QSize(200, 42));         //设置条目尺寸
    listWidget->setItemWidget(pListItem,pGroupItemWidget);
    vectorListWidgetItem.push_back(pListItem);

    return 0;
}

void GroupListOperation::add_sms_log_by_num(char *num, SMS_UNREAD smsLog)
{
    vector<QListWidgetItem *>::iterator iterItem;
    GroupItemWidget *pGroupItemWidget=NULL;


    for(iterItem=vectorListWidgetItem.begin();iterItem!=vectorListWidgetItem.end();iterItem++)
    {
        pGroupItemWidget=(GroupItemWidget *)listWidget->itemWidget(*iterItem);
        if(!strcmp(num,pGroupItemWidget->groupInfoData.groupNum))
        {
            pGroupItemWidget->vectorSMSLog.push_back(smsLog);
            cout <<"slot_add_sms_log"<<endl;
            return;
        }
    }
    return;
}

int GroupListOperation::fetch_sms_log_by_num(char *num, SMS_LOG *smsLog, int *n)
{
    GroupItemWidget *pGroupItemWidget=NULL;

    memset(smsLog,NULL,sizeof(SMS_LOG));

    pGroupItemWidget=get_item_widget_by_num(num);
    if(pGroupItemWidget==NULL)
    {
        cout <<"Group item is not exist"<<endl;
        return -1;
    }
    if(pGroupItemWidget->vectorSMSLog.empty())
    {
        cout <<"vectorSMSLog is empty"<<endl;
        return -2;
    }
    cout <<"fetch_sms_log_by_num"<<endl;
    if((pGroupItemWidget->vectorSMSLog.begin()+*n)!=pGroupItemWidget->vectorSMSLog.end())
        *smsLog=*(pGroupItemWidget->vectorSMSLog.begin()+*n);
    else
        return -2;
    (*n)++;

    return 0;
}

void GroupListOperation::slot_update_group_list_item(GROUP_INFO_DATA groupInfoData)
{
    update_item(groupInfoData);
    return;
}

void GroupListOperation::slot_double_clicked_item(QListWidgetItem *groupItem)
{
    GroupItemWidget *pGroupItemWidget=NULL;
    vector<QListWidgetItem *>::iterator iterItem;

    for(iterItem=vectorListWidgetItem.begin();iterItem!=vectorListWidgetItem.end();iterItem++)
    {
        if(*iterItem==groupItem)
        {
            pGroupItemWidget=(GroupItemWidget *)listWidget->itemWidget(groupItem);
            glp_ChatWindow->set_chat_item(&pGroupItemWidget->groupInfoData);
            glp_ChatWindow->show();
        }
    }

    return;
}

void GroupListOperation::slot_add_sms_log(GROUP_SMS_DATA smsData)
{
    vector<QListWidgetItem *>::iterator iterItem;
    GroupItemWidget *pGroupItemWidget=NULL;
    SMS_LOG smsLog;

    for(iterItem=vectorListWidgetItem.begin();iterItem!=vectorListWidgetItem.end();iterItem++)
    {
        pGroupItemWidget=(GroupItemWidget *)listWidget->itemWidget(*iterItem);
        if(!strcmp(pGroupItemWidget->groupInfoData.groupNum,smsData.toGroupNum))
        {
            if(strcmp(smsData.fromFriendInfo.num,gl_Server_Conn_Info.accountInfo.num))
            {
                memset(&smsLog,NULL,sizeof(smsLog));
                strcat(smsLog.sms,smsData.sms);
                sprintf(smsLog.head,"[%s] %s",smsData.fromFriendInfo.nickname,\
                        gl_Server_Conn_Info.current_time_string());
                pGroupItemWidget->vectorSMSLog.push_back(smsLog);
                cout <<"slot_add_sms_log"<<endl;
                return;
            }
            else
            {
                memset(&smsLog,NULL,sizeof(smsLog));
                strcat(smsLog.sms,smsData.sms);
                sprintf(smsLog.head,"[我] %s",gl_Server_Conn_Info.current_time_string());
                pGroupItemWidget->vectorSMSLog.push_back(smsLog);
                cout <<"slot_add_sms_log"<<endl;
                return;
            }
        }
    }
    return;
}













