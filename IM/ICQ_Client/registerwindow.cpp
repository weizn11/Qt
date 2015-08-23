#include <QDialog>
#include <QtGui>
#include <QtCore>

#include "registerwindow.h"
#include "ui_registerwindow.h"
#include "commun/commun.h"
#include "global.h"
#include "basic/encode.h"

extern ServerConn gl_Server_Conn_Info;

RegisterWindow::RegisterWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);    //将widget的标题栏和边框去掉

    ui->closeButton->setStyleSheet("QPushButton{border-image: url(:/images/button/res/close.png);}"
                                   "QPushButton:hover{border-image: url(:/images/button/res/closeP.png);}"
                                   "QPushButton:pressed{border-image: url(:/images/button/res/closeS.png);}");

    ui->minButton->setStyleSheet("QPushButton{border-image: url(:/images/button/res/narrow.png);}"
                                   "QPushButton:hover{border-image: url(:/images/button/res/narrowP.png);}"
                                   "QPushButton:pressed{border-image: url(:/images/button/res/narrowS.png);}");
}

RegisterWindow::~RegisterWindow()
{
    delete ui;
}

void RegisterWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(m_pixmapBg.rect(), m_pixmapBg);
    painter.drawPixmap(0,0,width(),height(),QPixmap(":/images/background/res/regBackground.jpg"));   //加载背景图片
}

void RegisterWindow::mouseMoveEvent(QMouseEvent *e)
{
    this->move(e->globalPos() - m_pointStart);
}

void RegisterWindow::mousePressEvent(QMouseEvent *e)
{
    m_pointPress = e->globalPos();
    m_pointStart = m_pointPress - this->pos();

}

void RegisterWindow::on_closeButton_clicked()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(500);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    QObject::connect(animation,SIGNAL(finished()),this,SLOT(close()));
}

void RegisterWindow::on_registerButton_clicked()
{
    REGISTER_ACCOUNT_DATA regData;
    QString QStrBuffer;

    memset(&regData,NULL,sizeof(REGISTER_ACCOUNT_DATA));

    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    ui->passwdStatusLabel->setPalette(pe);
    ui->regStatusLabel->setPalette(pe);
    ui->passwdStatusLabel->setText("");
    ui->passwdStatusLabel->repaint();
    ui->regStatusLabel->setText("");
    ui->regStatusLabel->repaint();

    QStrBuffer=ui->nicknameLineEdit->text();
    strcat(regData.nickname,QString_to_char(QStrBuffer));

    QStrBuffer=ui->truenameLineEdit->text();
    strcat(regData.truename,QString_to_char(QStrBuffer));

    QStrBuffer=ui->passwdLineEdit->text();
    if(QStrBuffer.isEmpty())
    {
        ui->passwdStatusLabel->setText("密码不可以为空！");
        return;
    }
    strcat(regData.password,QString_to_char(QStrBuffer));

    QStrBuffer=ui->confirmPassLineEdit->text();
    if(strcmp(regData.password,QString_to_char(QStrBuffer)))
    {
        ui->passwdStatusLabel->setText("两次密码输入不一致！");
        return;
    }

    QStrBuffer=ui->schoolLineEdit->text();
    strcat(regData.school,QString_to_char(QStrBuffer));

    QStrBuffer=ui->ageLineEdit->text();
    regData.age=atoi(QString_to_char(QStrBuffer));

    QStrBuffer=ui->sexComboBox->currentText();
    if(!strcmp("男",QString_to_char(QStrBuffer)))
    {
        regData.sex=1;
        cout <<"男"<<endl;
    }
    else
    {
        regData.sex=0;
    }

    if(gl_Server_Conn_Info.register_account(&regData)!=0)
    {
        ui->regStatusLabel->setText("系统异常或连接错误，请稍候再试...");
        return;
    }
    pe.setColor(QPalette::WindowText,Qt::black);
    ui->regStatusLabel->setPalette(pe);
    QStrBuffer.sprintf("账户注册成功，您的号码为：%s，请牢记！",regData.num);
    ui->regStatusLabel->setText(QStrBuffer);

    return;
}







