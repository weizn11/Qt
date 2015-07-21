#include "oneselfinfowindow.h"
#include "ui_oneselfinfowindow.h"
#include "mainwindow.h"

extern ServerConn gl_Server_Conn_Info;
extern MainWindow *pMainWindow;

OneselfInfoWindow::OneselfInfoWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OneselfInfoWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);    //将widget的标题栏和边框去掉
}

OneselfInfoWindow::~OneselfInfoWindow()
{
    delete ui;
}

void OneselfInfoWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(m_pixmapBg.rect(), m_pixmapBg);
    painter.drawPixmap(0,0,width(),height(),QPixmap(":/images/background/res/card1.jpg"));   //加载背景图片
}

void OneselfInfoWindow::mouseMoveEvent(QMouseEvent *e)
{
    this->move(e->globalPos() - m_pointStart);
}

void OneselfInfoWindow::mousePressEvent(QMouseEvent *e)
{
    m_pointPress = e->globalPos();
    m_pointStart = m_pointPress - this->pos();

}

void OneselfInfoWindow::ex_show()
{
    QPixmap *icon=NULL;
    QString qstr;

    if(access(gl_Server_Conn_Info.iconPath,0)!=0)
    {if(gl_Server_Conn_Info.accountInfo.sex==1)
    {
        icon=new QPixmap(":/images/portrait/res/boy.jpg");
    }
    else
    {
        icon=new QPixmap(":/images/portrait/res/girl.jpg");
    }}
    else
    {
        icon=new QPixmap(gl_Server_Conn_Info.iconPath);
    }
    ui->photoLabel->setPixmap(icon->scaled(111,101));

    ui->ageLineEdit->setText(qstr.sprintf("%d",gl_Server_Conn_Info.accountInfo.age));
    ui->nicknameLineEdit->setText(gl_Server_Conn_Info.accountInfo.nickname);
    ui->truenameLineEdit->setText(gl_Server_Conn_Info.accountInfo.truename);
    ui->signatureTextEdit->setText(gl_Server_Conn_Info.accountInfo.signature);
    ui->nameLabel->setText(gl_Server_Conn_Info.accountInfo.nickname);
    ui->accountLabel->setText(gl_Server_Conn_Info.accountInfo.num);
    ui->schoolLineEdit->setText(gl_Server_Conn_Info.accountInfo.school);
    if(gl_Server_Conn_Info.accountInfo.sex==1)
    {
        ui->sexComboBox->setCurrentIndex(0);
    }
    else
    {
        ui->sexComboBox->setCurrentIndex(1);
    }
    ui->statusLabel->setText("");
    ui->passwdLineEdit->setText("");
    ui->ackPasswdLineEdit->setText("");

    this->show();
}

void OneselfInfoWindow::ex_show(FRIEND_INFO_DATA friendInfoData)
{
    QPixmap *icon=NULL;
    QString qstr;

    qstr.sprintf("%s/%s.jpg",gl_Server_Conn_Info.tmpPath,friendInfoData.friendInfo.num);
    if(access(QString_to_char(qstr),0)!=0)
    {if(friendInfoData.friendInfo.sex==1)
    {
        icon=new QPixmap(":/images/portrait/res/boy.jpg");
    }
    else
    {
        icon=new QPixmap(":/images/portrait/res/girl.jpg");
    }}
    else
    {
        icon=new QPixmap(qstr);
    }
    ui->photoLabel->setPixmap(icon->scaled(111,101));

    ui->ageLineEdit->setText(qstr.sprintf("%d",friendInfoData.friendInfo.age));
    ui->ageLineEdit->setEnabled(false);
    ui->nicknameLineEdit->setText(friendInfoData.friendInfo.nickname);
    ui->nicknameLineEdit->setEnabled(false);
    ui->truenameLineEdit->setText(friendInfoData.friendInfo.truename);
    ui->truenameLineEdit->setEnabled(false);
    ui->signatureTextEdit->setText(friendInfoData.friendInfo.signature);
    ui->signatureTextEdit->setEnabled(false);
    ui->nameLabel->setText(friendInfoData.friendInfo.nickname);
    ui->accountLabel->setText(friendInfoData.friendInfo.num);
    ui->schoolLineEdit->setText(friendInfoData.friendInfo.school);
    ui->schoolLineEdit->setEnabled(false);
    if(friendInfoData.friendInfo.sex==1)
    {
        ui->sexComboBox->setCurrentIndex(0);
    }
    else
    {
        ui->sexComboBox->setCurrentIndex(1);
    }
    ui->sexComboBox->setEnabled(false);
    ui->statusLabel->setText("");
    ui->passwdLineEdit->close();
    ui->ackPasswdLineEdit->close();
    ui->uploadIconButton->close();
    ui->label_5->close();
    ui->label_6->close();
    ui->applyButton->close();

    this->show();
}

void OneselfInfoWindow::on_applyButton_clicked()
{
    QPixmap *icon=NULL;
    ACCOUNT_INFO_DATA accountData;
    QString QStrBuffer;

    memset(&accountData,NULL,sizeof(accountData));

    ui->statusLabel->setText("");
    ui->statusLabel->repaint();

    QStrBuffer=ui->nicknameLineEdit->text();
    strcat(accountData.nickname,QString_to_char(QStrBuffer));

    QStrBuffer=ui->truenameLineEdit->text();
    strcat(accountData.truename,QString_to_char(QStrBuffer));

    QStrBuffer=ui->passwdLineEdit->text();
    if(!QStrBuffer.isEmpty())
    {
        strcat(accountData.password,QString_to_char(QStrBuffer));

        QStrBuffer=ui->ackPasswdLineEdit->text();
        if(strcmp(accountData.password,QString_to_char(QStrBuffer)))
        {
            ui->statusLabel->setText("两次密码输入不一致！");
            return;
        }
    }

    QStrBuffer=ui->schoolLineEdit->text();
    strcat(accountData.school,QString_to_char(QStrBuffer));

    QStrBuffer=ui->ageLineEdit->text();
    accountData.age=atoi(QString_to_char(QStrBuffer));

    QStrBuffer=ui->sexComboBox->currentText();
    if(!strcmp("男",QString_to_char(QStrBuffer)))
    {
        accountData.sex=1;
    }
    else
    {
        accountData.sex=0;
    }

    QStrBuffer=ui->signatureTextEdit->toPlainText();
    strcat(accountData.signature,QString_to_char(QStrBuffer));

    switch (gl_Server_Conn_Info.update_account_data((&accountData)))
    {
    case -1:
        ui->statusLabel->setText("网络连接错误！");
        break;
    case -2:
        ui->statusLabel->setText("系统异常，请稍候再试！");
        break;
    case -3:
        ui->statusLabel->setText("响应超时，请稍候再试！");
        break;
    case 0:
        ui->statusLabel->setText("修改资料成功！");
        ui->nameLabel->setText(accountData.nickname);

        if(access(gl_Server_Conn_Info.iconPath,0)==0)
        {
            icon=new QPixmap(gl_Server_Conn_Info.iconPath);
        }
        else
        {if(gl_Server_Conn_Info.accountInfo.sex==1)
        {
            icon=new QPixmap(":/images/portrait/res/boy.jpg");
        }
        else
        {
            icon=new QPixmap(":/images/portrait/res/girl.jpg");
        }}
        ui->photoLabel->setPixmap(icon->scaled(111,101));

        pMainWindow->update_oneself_info();
        break;
    default:
        break;
    }

    return;
}

void OneselfInfoWindow::on_uploadIconButton_clicked()
{
    QString qstr;
    QPixmap *icon=NULL;
    char *pFilePath=NULL,*p=NULL;

    qstr=QFileDialog::getOpenFileName(this,"选择头像",".","*.jpg");
    if(qstr.isEmpty())
        return;
    pFilePath=QString_to_char(qstr);
    pFilePath=UTF8ToANSI(pFilePath);
    p=strchr(pFilePath,'.');
    if(p==NULL)
    {
        ui->statusLabel->setText("文件名不合法！");
        return;
    }
    if(strcmp(p,".jpg"))
    {
        ui->statusLabel->setText("文件名不合法！");
        return;
    }
    cout <<pFilePath<<endl;
    if(gl_Server_Conn_Info.upload_icon(pFilePath)==0)
    {
        ui->statusLabel->setText("头像上传成功.");
        icon=new QPixmap(qstr);
        ui->photoLabel->setPixmap(icon->scaled(111,101));
        Sleep(2000);
    }
    else
        ui->statusLabel->setText("文件打开失败！");
    pMainWindow->update_oneself_info();

    return;
}










