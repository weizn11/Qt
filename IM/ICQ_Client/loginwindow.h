#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QtGui>
#include <QtCore>

#include "mainwindow.h"
#include "registerwindow.h"
#include "global.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();
protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);  

private slots:
    void on_loginButton_clicked();
    void on_closeButton_clicked();
    void on_registerButton_clicked();
    void qt_exit();

private:
    Ui::LoginWindow *ui;

    QPalette palette;

    QPixmap m_pixmapBg;
    QAction *m_AactionAboutQt;
    QPoint m_pointStart;
    QPoint m_pointPress;

    RegisterWindow registerWindow;
};

#endif // LOGINWINDOW_H
