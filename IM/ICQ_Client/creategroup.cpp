#include "creategroup.h"
#include "ui_creategroup.h"
#include "commun/commun.h"

extern ServerConn gl_Server_Conn_Info;

CreateGroup::CreateGroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateGroup)
{
    ui->setupUi(this);
    QWidget::setFixedSize(this->width(),this->height());     //固定窗口大小
}

void CreateGroup::show()
{
    ui->groupIntroEdit->setText("");
    ui->groupNameEdit->setText("");
    ui->statusLabel->setText("");
    QWidget::show();
    return;
}

CreateGroup::~CreateGroup()
{
    delete ui;
}

void CreateGroup::on_createButton_clicked()
{
    QString qstr;
    CREATE_GROUP_DATA createGroupData;

    memset(&createGroupData,NULL,sizeof(createGroupData));

    qstr=ui->groupNameEdit->text();
    if(qstr.isEmpty())
    {
        ui->statusLabel->setText("群名称不能为空！");
        return;
    }
    strcat(createGroupData.groupName,QString_to_char(qstr));
    qstr=ui->groupIntroEdit->toPlainText();
    strcat(createGroupData.groupIntro,QString_to_char(qstr));

    switch(gl_Server_Conn_Info.create_group(&createGroupData))
    {
    case 0:
        qstr.sprintf("聊天群创建成功！群号为：%s.",createGroupData.groupNum);
        ui->statusLabel->setText(qstr);
        break;
    case -1:
        ui->statusLabel->setText("网络连接错误！");
        break;
    case -2:
        ui->statusLabel->setText("系统异常，请稍后再试。");
        break;
    case -3:
        ui->statusLabel->setText("应答超时，请稍后再试。");
        break;
    }
    return;
}



