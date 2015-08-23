#include "findfriendwindow.h"
#include "ui_findfriendwindow.h"
#include "commun/commun.h"

extern ServerConn gl_Server_Conn_Info;

FindFriendWindow::FindFriendWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindFriendWindow)
{
    ui->setupUi(this);

    move(450,150);
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    ui->statusLabel->setPalette(pe);

    //设置label标签自动换行
    ui->signatureLabel->setWordWrap(true);
    ui->signatureLabel->setAlignment(Qt::AlignTop);
}

FindFriendWindow::~FindFriendWindow()
{
    delete ui;
}

void FindFriendWindow::on_findButton_clicked()
{
    QString qstr;
    ACCOUNT_INFO_DATA accountInfo;

    memset(&accountInfo,NULL,sizeof(ACCOUNT_INFO_DATA));

    this->set_window_size(370,80);
     ui->statusLabel_2->setText("");
    clear_statusLabel();
    qstr=ui->findNumLineEdit->text();
    strcat(accountInfo.num,QString_to_char(qstr));
    cout <<accountInfo.num<<endl;
    switch(gl_Server_Conn_Info.find_friend(&accountInfo))
    {
    case 0:
        break;
    case -1:
        ui->statusLabel->setText("网络连接错误！");
        return;
    case -2:
        ui->statusLabel->setText("查询超时，请稍候再试...");
        return;
    case -3:
        ui->statusLabel->setText("不存在此账号！");
        return;
    }

    qstr.sprintf("账号：%s",accountInfo.num);
    ui->numLabel->setText(qstr);

    qstr.sprintf("昵称：%s",accountInfo.nickname);
    ui->nicknameLabel->setText(qstr);

    qstr.sprintf("真实姓名：%s",accountInfo.truename);
    ui->truenameLabel->setText(qstr);

    qstr.sprintf("性别：%s",accountInfo.sex==1?"男":"女");
    ui->sexLabel->setText(qstr);

    qstr.sprintf("年龄：%d",accountInfo.age);
    ui->ageLabel->setText(qstr);

    qstr.sprintf("学校：%s",accountInfo.school);
    ui->schoolLabel->setText(qstr);

    qstr.sprintf("个人签名：%s",accountInfo.signature);
    ui->signatureLabel->setAlignment(Qt::AlignLeft);
    ui->signatureLabel->setText(qstr);

    this->set_window_size(370,484);   //设置窗口大小
    return;
}

void FindFriendWindow::clear_statusLabel()
{
    ui->statusLabel->setText("");
    ui->statusLabel->repaint();
    return;
}

void FindFriendWindow::set_window_size(unsigned int wide,unsigned int high)
{
    this->resize(QSize(wide,high));
    QWidget::setFixedSize(QSize(wide,high));
    return;
}

void FindFriendWindow::on_addFriendButton_clicked()
{
    QString qstr;
    QPalette pe;
    FRIENDSHIP_INFO_DATA friendshipInfo;
    char *pch=NULL;

    memset(&friendshipInfo,NULL,sizeof(friendshipInfo));

    strcat(friendshipInfo.num1,gl_Server_Conn_Info.accountInfo.num);
    qstr=ui->numLabel->text();
    pch=QString_to_char(qstr);
    pch=strchr(pch,'：');
    if(pch==NULL)
        return;
    pch++;
    strcat(friendshipInfo.num2,pch);
    ui->statusLabel_2->setAlignment(Qt::AlignCenter);
    switch(gl_Server_Conn_Info.add_friend(&friendshipInfo))
    {
    case 0:
        pe.setColor(QPalette::WindowText,Qt::green);
        ui->statusLabel_2->setPalette(pe);
        ui->statusLabel_2->setText("添加好友成功！");
        break;
    case -1:
        pe.setColor(QPalette::WindowText,Qt::red);
        ui->statusLabel_2->setPalette(pe);
        ui->statusLabel_2->setText("添加好友失败，原因：网络连接错误！");
        break;
    case -2:
        pe.setColor(QPalette::WindowText,Qt::red);
        ui->statusLabel_2->setPalette(pe);
        ui->statusLabel_2->setText("添加好友失败，原因：系统异常！");
        break;
    case -3:
        pe.setColor(QPalette::WindowText,Qt::red);
        ui->statusLabel_2->setPalette(pe);
        ui->statusLabel_2->setText("添加好友失败，原因：你们已经是好友！");
        break;
    }

    return;
}












