#ifndef FRIENDSTREEVIEW_H
#define FRIENDSTREEVIEW_H

#include "global.h"
#include "chatform.h"
#include "oneselfinfowindow.h"

class FriendItemWidget : public QWidget
{
    Q_OBJECT
public:
    FILE *pFile;
    FRIEND_INFO_DATA friendInfoData;
    FriendItemWidget();
    FriendItemWidget(FRIEND_INFO_DATA *pFriendInfoData);
    int setItem(FRIEND_INFO_DATA *pFriendInfoData);
    QPixmap convertToGray(QImage image);     //图片置灰
    char *get_item_account();
    int change_labelName_color(char *color);
signals:

public slots:

public:
    vector<SMS_LOG> vectorSMSLog;
    vector<SMS_UNREAD> vectorSMSUnread;
private:
    QLabel *labelIcon;
    QLabel *labelName;
    QLabel *labelInfo;

    QHBoxLayout *horLayout;
    QVBoxLayout *verlayout;

protected:
    bool event(QEvent *e);
};

class FriendTreeOperation : public QWidget
{
    Q_OBJECT
public:
    FriendTreeOperation(QWidget *parent);
    int update_friend_item(FRIEND_INFO_DATA *pFriendInfoData);
    FriendItemWidget *get_item_widget_by_num(char *num);
    int fetch_sms_unread_by_num(char *num,SMS_UNREAD *smsUnread);
    int fetch_sms_log_by_num(char *num,SMS_LOG *smsLog,int *n);
    int sms_unread_remind(char *num);
    int sms_unread_close_remind(char *num);
    void add_sms_log_by_num(char *num,SMS_UNREAD smsLog);
    int delete_friend(char *num);
signals:
    ;

public slots:
    void slot_update_friend_item(FRIEND_INFO_DATA FriendInfoData);
    void slot_update_item_icon(char *num);
    void slot_double_clicked_item(QTreeWidgetItem *friendItem,int index);
    void slot_add_sms_unread(SMS_DATA smsData);
    void slot_add_sms_log(SMS_DATA smsData);
    void slot_delete_friend(char *num);

public:
    QTreeWidget *treeWidget;
    vector<QTreeWidgetItem *> vectorFriendGroup,vectorFriendItem;
    OneselfInfoWindow *pFriendInfoCard;
};

#endif // FRIENDSTREEVIEW_H







