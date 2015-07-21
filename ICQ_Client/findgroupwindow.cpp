#include "findgroupwindow.h"
#include "ui_findgroupwindow.h"
#include "commun/commun.h"

extern ServerConn gl_Server_Conn_Info;

FindGroupWindow::FindGroupWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindGroupWindow)
{
    ui->setupUi(this);

    //设置label标签自动换行
    ui->groupIntroLabel->setWordWrap(true);

    set_window_size(361,93);
    move(450,150);
    return;
}

FindGroupWindow::~FindGroupWindow()
{
    delete ui;
}

void FindGroupWindow::set_window_size(unsigned int wide, unsigned int high)
{
    this->resize(QSize(wide,high));
    QWidget::setFixedSize(QSize(wide,high));
    return;
}

void FindGroupWindow::on_findButton_clicked()
{
    QString qstr;
    GROUP_INFO_DATA groupInfoData;

    memset(&groupInfoData,NULL,sizeof(groupInfoData));

    set_window_size(361,93);
    ui->findStatusLabel->setText("");
    ui->findStatusLabel->repaint();
    ui->joinStatusLabel->setText("");
    ui->joinStatusLabel->repaint();
    qstr=ui->groupNumEdit->text();
    strcat(groupInfoData.groupNum,QString_to_char(qstr));
    switch(gl_Server_Conn_Info.find_group(&groupInfoData))
    {
    case 0:
        qstr.sprintf("群号：%s",groupInfoData.groupNum);
        ui->groupNumLabel->setText(qstr);
        qstr.sprintf("群名称：%s",groupInfoData.groupName);
        ui->groupNameLabel->setText(qstr);
        qstr.sprintf("群简介：%s",groupInfoData.groupIntro);
        ui->groupIntroLabel->setText(qstr);
        set_window_size(361,361);
        break;
    case -1:
        ui->findStatusLabel->setText("网络连接错误！");
        break;
    case -2:
        ui->findStatusLabel->setText("不存在此聊天群！");
        break;
    case -3:
        ui->findStatusLabel->setText("系统异常，请稍候再试！");
        break;
    case -4:
        ui->findStatusLabel->setText("查询超时，请稍候再试！");
        break;
    }

    return;
}

void FindGroupWindow::on_joinButton_clicked()
{
    QString qstr;
    char *chstr=NULL;
    QPalette pe;
    GROUP_INFO_DATA groupInfoData;

    memset(&groupInfoData,NULL,sizeof(groupInfoData));

    qstr=ui->groupNumLabel->text();
    chstr=strchr(QString_to_char(qstr),'：');
    chstr++;
    if(strlen(chstr)<1)
        return;
    strcat(groupInfoData.groupNum,chstr);

    pe.setColor(QPalette::WindowText,Qt::red);
    ui->joinStatusLabel->setPalette(pe);
    switch(gl_Server_Conn_Info.join_group(&groupInfoData))
    {
    case 0:
        pe.setColor(QPalette::WindowText,Qt::green);
        ui->joinStatusLabel->setPalette(pe);
        ui->joinStatusLabel->setText("成功加入聊天群！");
        break;
    case -1:
        ui->joinStatusLabel->setText("网络连接错误！");
        break;
    case -2:
        ui->joinStatusLabel->setText("加入聊天群失败！原因：你已加入该聊天群。");
        break;
    case -3:
        ui->joinStatusLabel->setText("加入聊天群失败！原因：系统异常。");
        break;
    case -4:
        ui->joinStatusLabel->setText("加入聊天群失败！原因：应答超时。");
        break;
    }

    return;
}















