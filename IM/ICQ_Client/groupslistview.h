#ifndef GROUPSLISTVIEW
#define GROUPSLISTVIEW

#include "global.h"

class GroupItemWidget : public QWidget
{
    Q_OBJECT
public:
    GroupItemWidget(GROUP_INFO_DATA groupInfoData);
public:
    GROUP_INFO_DATA groupInfoData;
    vector<SMS_LOG> vectorSMSLog;
private:
    QLabel *labelIcon;
    QLabel *labelName;
    QHBoxLayout *horLayout;
};

class GroupListOperation : public QWidget
{
    Q_OBJECT
public:
    GroupListOperation(QWidget *parent);
    GroupItemWidget *get_item_widget_by_num(char *num);
    int update_item(GROUP_INFO_DATA groupInfoData);
    void add_sms_log_by_num(char *num,SMS_UNREAD smsLog);
    int fetch_sms_log_by_num(char *num,SMS_LOG *smsLog,int *n);
private:
    QListWidget *listWidget;
    vector<QListWidgetItem *> vectorListWidgetItem;
public slots:
    void slot_update_group_list_item(GROUP_INFO_DATA groupInfoData);
    void slot_double_clicked_item(QListWidgetItem *groupItem);
    void slot_add_sms_log(GROUP_SMS_DATA smsData);
};

#endif // GROUPSLISTVIEW

