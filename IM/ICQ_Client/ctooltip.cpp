#include "ctooltip.h"
#include "commun/commun.h"
#include "mainwindow.h"

extern ServerConn gl_Server_Conn_Info;
CToolTip *glp_CToolTip;

CToolTip::CToolTip(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    this->resize(200, 100); ;

    this->setObjectName("CToolTip");
    this->setStyleSheet("QWidget#CToolTip {border: 2px solid green; background-color: skyblue;}");

    groupBox = new QGroupBox(this);
    groupBox->setGeometry(10, 10, 180, 80);
    groupBox->setTitle("好友信息");

    labelIcon = new QLabel(groupBox);
    labelName = new QLabel(groupBox);
    labelInfo = new QLabel(groupBox);

    verlayout = new QVBoxLayout();
    verlayout->setContentsMargins(0, 0, 0, 0);
    verlayout->addWidget(labelName);
    verlayout->addWidget(labelInfo);

    horLayout = new QHBoxLayout(groupBox);
    horLayout->setContentsMargins(10, 10, 10, 10);
    horLayout->addWidget(labelIcon, 1, Qt::AlignTop);
    horLayout->addLayout(verlayout, 4);
}

// 显示ToolTip消息
void CToolTip::showMessage(FRIEND_INFO_DATA friendInfoData, QPoint point)
{
    char iconPath[1000];
    QPixmap icon;
    QString qstr;

    memset(iconPath,NULL,sizeof(iconPath));
    strcat(iconPath,gl_Server_Conn_Info.tmpPath);
    strcat(iconPath,friendInfoData.friendInfo.num);
    strcat(iconPath,".jpg");
    if(access(iconPath,0)!=0)
    {
        if(friendInfoData.friendInfo.sex==1)
        {
            icon=QPixmap(QString(":/images/portrait/res/boy.jpg"));
            labelIcon->setPixmap(icon.scaled(43,40));
        }
        else
        {
            icon=QPixmap(QString(":/images/portrait/res/girl.jpg"));
            labelIcon->setPixmap(icon.scaled(43,40));
        }
    }
    else
    {
        icon=QPixmap(iconPath);
        labelIcon->setPixmap(icon.scaled(43,40));
    }
    labelName->setText(qstr.sprintf("%s(%s)",friendInfoData.friendInfo.nickname,friendInfoData.friendInfo.num));
    labelInfo->setText(qstr.sprintf("姓名：%s",friendInfoData.friendInfo.truename));

    // 重新定义CToolTip的坐标
    int rectX;
    int rectY;
    if (point.rx() < 200) {
        rectX = point.rx() + 10;
    } else {
        rectX = point.rx() - 240;
    }
    rectY =  point.ry();
    move(QPoint(rectX, rectY));
    QWidget::show();
}

// 当鼠标进入事件时，让界面隐藏掉
void CToolTip::hoverEvent(QHoverEvent *)
{
    //hide();
}

