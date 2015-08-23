#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "global.h"
#include "findfriendwindow.h"
#include "friendstreeview.h"
#include "oneselfinfowindow.h"
#include "creategroup.h"
#include "findgroupwindow.h"

namespace Ui {
class MainWindow;
}

class FriendItemMenu : public QMenu
{
    Q_OBJECT
public:
    FriendItemMenu(QWidget *parent = 0);
public:
    FriendItemWidget *pFriendItemWidget;
public slots:
    void slot_friend_item_triggered(QAction * action);
private:
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void update_oneself_info();
protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);

private slots:
    void on_closeButton_clicked();
    void qt_exit();
    void on_changePhotoButton_clicked();
    void on_findUserButton_clicked();
    void on_photoButton_clicked();
    void slot_update_myself_icon();
    void on_friendsTreeWidget_customContextMenuRequested(const QPoint &pos);
    void on_createGroupButton_clicked();
    void on_findGroupButton_clicked();

    void on_onlineComboBox_currentTextChanged(const QString &arg1);

public:
    FindFriendWindow findFriWindow;
    OneselfInfoWindow myInfoWindow;
private:
    Ui::MainWindow *ui;

    QPixmap m_pixmapBg;
    QAction *m_AactionAboutQt;
    QPoint m_pointStart;
    QPoint m_pointPress;
    QString backgroundPathStr;

    FriendItemMenu *friendItemMenu;
    CreateGroup *pCreateGroup;
    FindGroupWindow *pFindGroupWindow;
};

#endif // MAINWINDOW_H
