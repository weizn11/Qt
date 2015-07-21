#include "loginwindow.h"
#include "mainwindow.h"
#include "ui_loginwindow.h"
#include "commun/commun.h"
#include "global.h"
#include "basic/encode.h"

extern MainWindow *pMainWindow;
extern ServerConn gl_Server_Conn_Info;

LoginWindow::LoginWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);    //将widget的标题栏和边框去掉

    ui->loginButton->setStyleSheet("QPushButton{border-image: url(:/images/button/res/lan.png);}"
                                   "QPushButton:hover{border-image: url(:/images/button/res/lan2.png);}"
                                   "QPushButton:pressed{border-image: url(:/images/button/res/lan3.png);}");

    ui->closeButton->setStyleSheet("QPushButton{border-image: url(:/images/button/res/close.png);}"
                                   "QPushButton:hover{border-image: url(:/images/button/res/closeP.png);}"
                                   "QPushButton:pressed{border-image: url(:/images/button/res/closeS.png);}");

    ui->minButton->setStyleSheet("QPushButton{border-image: url(:/images/button/res/narrow.png);}"
                                   "QPushButton:hover{border-image: url(:/images/button/res/narrowP.png);}"
                                   "QPushButton:pressed{border-image: url(:/images/button/res/narrowS.png);}");

    //淡出窗口
     QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
     animation->setDuration(1000);
     animation->setStartValue(0);
     animation->setEndValue(1);
     animation->start();
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(m_pixmapBg.rect(), m_pixmapBg);
    painter.drawPixmap(0,0,width(),height(),QPixmap(":/images/background/res/lan7.jpg"));   //加载背景图片
}

void LoginWindow::mouseMoveEvent(QMouseEvent *e)
{
    this->move(e->globalPos() - m_pointStart);
}

void LoginWindow::mousePressEvent(QMouseEvent *e)
{
    m_pointPress = e->globalPos();
    m_pointStart = m_pointPress - this->pos();

}

void LoginWindow::on_loginButton_clicked()
{
    QString accountStr,passwordStr;

    accountStr=ui->accountLineEdit->text();
    passwordStr=ui->passwordLineEdit->text();

    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    ui->loginStatusLabel->setPalette(pe);

    ui->loginStatusLabel->setText("正在登录...");
    ui->loginStatusLabel->repaint();

    if(gl_Server_Conn_Info.connect_to_server()!=0)
    {
        ui->loginStatusLabel->setText("网络连接错误！");
        return;
    }
    switch(gl_Server_Conn_Info.auth_account(QString_to_char(accountStr),QString_to_char(passwordStr)))
    {
    case 0:
        break;
    case -1:
        ui->loginStatusLabel->setText("账号已登录！");
        return;
    default:
        ui->loginStatusLabel->setText("用户名或密码错误！");
        return;
    }
    pe.setColor(QPalette::WindowText,Qt::green);
    ui->loginStatusLabel->setPalette(pe);
    ui->loginStatusLabel->setText("登录成功！");
    ui->loginStatusLabel->repaint();
    pMainWindow->update_oneself_info();

    QPropertyAnimation *animation1 = new QPropertyAnimation(pMainWindow, "windowOpacity");
    animation1->setDuration(1000);
    animation1->setStartValue(0);
    animation1->setEndValue(1);
    animation1->start();
    pMainWindow->show();
    QPropertyAnimation *animation2 = new QPropertyAnimation(this, "windowOpacity");
    animation2->setDuration(1000);
    animation2->setStartValue(1);
    animation2->setEndValue(0);
    animation2->start();
    QObject::connect(animation2,SIGNAL(finished()),this,SLOT(close()));
}

void LoginWindow::on_closeButton_clicked()
{
    QPropertyAnimation *animation2 = new QPropertyAnimation(&registerWindow, "windowOpacity");
    animation2->setDuration(1000);
    animation2->setStartValue(1);
    animation2->setEndValue(0);
    animation2->start();
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    QObject::connect(animation, SIGNAL(finished()), this, SLOT(qt_exit()));
}

void LoginWindow::on_registerButton_clicked()
{
    //淡出窗口
    QPropertyAnimation *animation = new QPropertyAnimation(&registerWindow, "windowOpacity");
    animation->setDuration(500);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
    registerWindow.show();
}

void LoginWindow::qt_exit()
{
    exit(0);
}
