#ifndef CHATFORM_H
#define CHATFORM_H

#include <QWidget>
#include "global.h"
#include "filesendform.h"
#include "filerecvform.h"
#include "commun/commun.h"

namespace Ui {
class ChatForm;
}

class ChatForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChatForm(QWidget *parent = 0);
    void set_info(FRIEND_INFO_DATA *pFriendInfoData);
    void set_info(GROUP_INFO_DATA *pGroupInfoData);
    void show_file_send_widget();
    void close_file_send_widget();
    void show_file_recv_widget();
    void close_file_recv_widget();
    void append_text(SMS_DATA smsData);
    void append_text(GROUP_SMS_DATA groupSMSData);
    ~ChatForm();
protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
signals:

private slots:
    void on_minButton_clicked();
    void on_closeAllButton_clicked();
    void on_closeCurrentButton_clicked();
    void on_sendButton_clicked();
    void on_sendFileButton_clicked();
public:
    FileSendForm *fileSendForm;
    FileRecvForm *fileRecvForm;
private:
    Ui::ChatForm *ui;

    QPixmap m_pixmapBg;
    QAction *m_AactionAboutQt;
    QPoint m_pointStart;
    QPoint m_pointPress;
};

#endif // CHATFORM_H
