#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"
#include "commun/commun.h"
#include "oneselfinfowindow.h"
#include "mainwindow.h"
#include "ctooltip.h"
#include "creategroup.h"
#include "groupslistview.h"

extern FriendTreeOperation *gl_FriendTree_Operation;
extern RecvThread *recvThread;
extern ServerConn gl_Server_Conn_Info;
extern CToolTip *glp_CToolTip;
extern GroupListOperation *glp_GroupListOperation;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    gl_FriendTree_Operation=new FriendTreeOperation(ui->friendsTreeWidget);
    glp_GroupListOperation=new GroupListOperation(ui->groupListWidget);
    recvThread=new RecvThread();
    qRegisterMetaType<FRIEND_INFO_DATA>("FRIEND_INFO_DATA");    //注册该类型
    qRegisterMetaType<char *>("char *");
    qRegisterMetaType<GROUP_INFO_DATA>("GROUP_INFO_DATA");
    connect(recvThread,SIGNAL(signal_update_friend_item(FRIEND_INFO_DATA)),gl_FriendTree_Operation,\
                SLOT(slot_update_friend_item(FRIEND_INFO_DATA)));
    connect(recvThread,SIGNAL(signal_update_item_icon(char *)),gl_FriendTree_Operation,\
                SLOT(slot_update_item_icon(char *)));
    connect(ui->friendsTreeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),gl_FriendTree_Operation,\
            SLOT(slot_double_clicked_item(QTreeWidgetItem*,int)));
    connect(ui->groupListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),glp_GroupListOperation,\
            SLOT(slot_double_clicked_item(QListWidgetItem*)));
    connect(recvThread,SIGNAL(signal_update_myself_icon()),this,SLOT(slot_update_myself_icon()));
    connect(recvThread,SIGNAL(signal_update_group_list_item(GROUP_INFO_DATA)),glp_GroupListOperation,\
            SLOT(slot_update_group_list_item(GROUP_INFO_DATA)));

    backgroundPathStr.sprintf(":/images/background/res/mainBG%d.jpg",1);
    move(900,80);           //设置窗口出现位置
    setWindowFlags(Qt::FramelessWindowHint);    //将widget的标题栏和边框去掉

    ui->closeButton->setStyleSheet("QPushButton{border-image: url(:/images/button/res/close.png);}"
                                   "QPushButton:hover{border-image: url(:/images/button/res/closeP.png);}"
                                   "QPushButton:pressed{border-image: url(:/images/button/res/closeS.png);}");

    ui->minButton->setStyleSheet("QPushButton{border-image: url(:/images/button/res/narrow.png);}"
                                   "QPushButton:hover{border-image: url(:/images/button/res/narrowP.png);}"
                                   "QPushButton:pressed{border-image: url(:/images/button/res/narrowS.png);}");

    ui->commTabWidget->setStyleSheet("QTabWidget:pane {border-top:0px solid #e8f3f9;background-color:rgba(255,255,255,100)}\
                                      QTabBar::tab{width:70}\
                                      QTabBar::tab:pane {background-color:rgba(255,255,255,200)}\
                                      QTabBar::tab:selected {background-color:rgba(255,255,255,100)}\
                                      QTabBar::tab:!selected {background-color:rgba(255,255,255,200)}");

    ui->groupTab->setStyleSheet("background-color:rgba(255,255,255,0)");
    ui->friendsTab->setStyleSheet("background-color:rgba(255,255,255,0)");
    ui->onlineComboBox->setStyleSheet("background-color:rgba(255,255,255,100)");
    ui->photoButton->setStyleSheet("background-color:rgba(255,255,255,100)");
    ui->findGroupButton->setStyleSheet("background-color:rgba(255,255,255,100)");
    ui->createGroupButton->setStyleSheet("background-color:rgba(255,255,255,100)");
    ui->findUserButton->setStyleSheet("background-color:rgba(255,255,255,100)");
    ui->changePhotoButton->setStyleSheet("background-color:rgba(255,255,255,50)");

    friendItemMenu=new FriendItemMenu(this);
    glp_CToolTip=new CToolTip();
    pCreateGroup=new CreateGroup();
    pFindGroupWindow=new FindGroupWindow();

    return;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_oneself_info()
{
    QString QIconSheet;

    if(access(gl_Server_Conn_Info.iconPath,0)==0)
    {
        cout <<"icon file exist,the path is:"<<gl_Server_Conn_Info.iconPath<<endl;
        QIconSheet.sprintf("QPushButton{border-image: url(%s);}",gl_Server_Conn_Info.iconPath);
        ui->photoButton->setStyleSheet(QIconSheet);
    }
    else
    {
        if(gl_Server_Conn_Info.accountInfo.sex==1)
            ui->photoButton->setStyleSheet("QPushButton{border-image: url(:/images/portrait/res/boy.jpg);}");
        else
            ui->photoButton->setStyleSheet("QPushButton{border-image: url(:/images/portrait/res/girl.jpg);}");
    }
    ui->nameLabel->setText(gl_Server_Conn_Info.accountInfo.nickname);

    return;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(m_pixmapBg.rect(), m_pixmapBg);
    painter.drawPixmap(0,0,width(),height(),QPixmap(backgroundPathStr));   //加载背景图片
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    this->move(e->globalPos() - m_pointStart);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    m_pointPress = e->globalPos();
    m_pointStart = m_pointPress - this->pos();

}

void MainWindow::on_closeButton_clicked()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    QObject::connect(animation, SIGNAL(finished()), this, SLOT(qt_exit()));
}

void MainWindow::qt_exit()
{
    exit(0);
}

void MainWindow::on_changePhotoButton_clicked()
{
    static int n=1;

    n>=10?n=1:n++;
    backgroundPathStr.sprintf(":/images/background/res/mainBG%d.jpg",n);
    //淡出窗口
    QPropertyAnimation *animation1 = new QPropertyAnimation(this, "windowOpacity");
    animation1->setDuration(500);
    animation1->setStartValue(0.7);
    animation1->setEndValue(1);
    animation1->start();
    this->repaint();
}

void MainWindow::on_findUserButton_clicked()
{
    findFriWindow.clear_statusLabel();
    findFriWindow.set_window_size(370,80);
    findFriWindow.show();
}

void MainWindow::on_photoButton_clicked()
{
    myInfoWindow.ex_show();
}

void MainWindow::slot_update_myself_icon()
{
    this->update_oneself_info();
    return;
}

void MainWindow::on_friendsTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    FriendItemWidget *pFriendItemWidget=NULL;
    vector<QTreeWidgetItem *>::iterator iterItem;
    QTreeWidgetItem *curItem=ui->friendsTreeWidget->itemAt(pos);  //获取当前被点击的节点

    if(curItem==NULL) return;           //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    for(iterItem=gl_FriendTree_Operation->vectorFriendItem.begin();\
        iterItem!=gl_FriendTree_Operation->vectorFriendItem.end();iterItem++)
    {
        if(*iterItem==curItem)
            break;
    }
    if(iterItem==gl_FriendTree_Operation->vectorFriendItem.end())
        return;
    pFriendItemWidget=(FriendItemWidget *)ui->friendsTreeWidget->itemWidget(curItem,0);
    friendItemMenu->pFriendItemWidget=pFriendItemWidget;

    friendItemMenu->exec(QCursor::pos());//弹出右键菜单，菜单位置为光标位置

    return;
}

FriendItemMenu::FriendItemMenu(QWidget *parent) : QMenu(parent)
{
    this->addAction("查看资料");
    this->addAction("删除好友");
    connect(this,SIGNAL(triggered(QAction*)),this,SLOT(slot_friend_item_triggered(QAction*)));
}

void FriendItemMenu::slot_friend_item_triggered(QAction *action)
{
    QString qstr;
    char *chstr=NULL,*p=NULL;

    qstr=action->text();
    chstr=QString_to_char(qstr);
    if((p=strstr(chstr,"删除好友")))
    {
        QMessageBox::StandardButton rb = QMessageBox::question(NULL, "删除确认", "确定删除此好友？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if(rb == QMessageBox::Yes)
        {
            qDebug() <<"delete:"<<pFriendItemWidget->friendInfoData.friendInfo.num;
            gl_FriendTree_Operation->delete_friend(pFriendItemWidget->friendInfoData.friendInfo.num);
        }
    }
    else if((p=strstr(chstr,"查看资料")))
    {
        qDebug() <<"查看资料";
        gl_FriendTree_Operation->pFriendInfoCard->ex_show(pFriendItemWidget->friendInfoData);
    }

    return;
}

void MainWindow::on_createGroupButton_clicked()
{
    pCreateGroup->show();
}

void MainWindow::on_findGroupButton_clicked()
{
    pFindGroupWindow->set_window_size(361,93);
    pFindGroupWindow->show();
    return;
}

void MainWindow::on_onlineComboBox_currentTextChanged(const QString &arg1)
{
    char *chstr=NULL;

    qDebug() <<arg1;
    chstr=QString_to_char(arg1);
    if(strstr(chstr,"在线"))
    {
        gl_Server_Conn_Info.change_online_status(1);
    }
    else
    {
        gl_Server_Conn_Info.change_online_status(0);
    }

    return;
}










