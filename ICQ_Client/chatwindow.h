#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QDialog>
#include <vector>
#include "global.h"
#include "chatform.h"

namespace Ui {
class ChatWindow;
}
class ChatItemWidget;

class ChatWindow : public QDialog
{
    Q_OBJECT
public:
    explicit ChatWindow(QWidget *parent = 0);
    void set_chat_item(FRIEND_INFO_DATA *pFriendInfoData);
    void set_chat_item(GROUP_INFO_DATA *pGroupInfoData);
    void remove_chat_item(ChatForm *pChatForm);
    void clear_chat_item();
    ChatItemWidget *search_chat_item_by_num(char *num);
    ChatItemWidget *search_group_chat_item_by_num(char *num);
    ~ChatWindow();
protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
public slots:
    void slot_change_chat_form(QListWidgetItem *current, QListWidgetItem *previous);
    void slot_show_sms_on_text(SMS_DATA smsData);
    void slot_show_sms_on_text(GROUP_SMS_DATA smsData);
    void slot_recv_file(FILE_TRAN_DATA fileTranData);
    void slot_reject_recv_file(FILE_TRAN_DATA fileTranData);
    void slot_start_tran_file(FILE_TRAN_DATA fileTranData);
    void slot_update_send_info(int done,int total,char *num);
    void slot_update_recv_info(int done,int total,char *num);
public:
    QPixmap m_pixmapBg;
    QAction *m_AactionAboutQt;
    QPoint m_pointStart;
    QPoint m_pointPress;
private:
    Ui::ChatWindow *ui;

    vector<QListWidgetItem *> vectorListItem;
};

class ChatItemWidget : public QWidget
{
    Q_OBJECT
public:
    ChatItemWidget(FRIEND_INFO_DATA *pFriendInfoData);
    ChatItemWidget(GROUP_INFO_DATA *pGroupInfoData);
    char *get_item_account();
public:
    FRIEND_INFO_DATA friendInfoData;
    GROUP_INFO_DATA groupInfoData;
signals:

public slots:

public:
    ChatForm *pChatForm;
private:
    QLabel *labelIcon;
    QLabel *labelName;
    QLabel *labelInfo;

    QHBoxLayout *horLayout;
    QVBoxLayout *verlayout;
protected:
};

#endif // CHATWINDOW_H
