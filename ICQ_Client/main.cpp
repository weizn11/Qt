#include <QApplication>
#include <iostream>
#include <conio.h>

#include "mainwindow.h"
#include "loginwindow.h"

#include "global.h"
#include "p2p/p2p.h"
#include "commun/commun.h"
#include "chatwindow.h"

MainWindow *pMainWindow;
ServerConn gl_Server_Conn_Info;
extern ChatWindow *glp_ChatWindow;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mainWindow;
    LoginWindow loginWindow;
    ChatWindow chatWindow;
    glp_ChatWindow=&chatWindow;
    pMainWindow=&mainWindow;


    //chatWindow.show();

    loginWindow.show();

    return a.exec();
}
