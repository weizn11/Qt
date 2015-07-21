#ifndef FINDFRIENDWINDOW_H
#define FINDFRIENDWINDOW_H

#include <QDialog>
#include "global.h"

namespace Ui {
class FindFriendWindow;
}

class FindFriendWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FindFriendWindow(QWidget *parent = 0);
    ~FindFriendWindow();
    void set_window_size(unsigned int wide,unsigned int high);
    void clear_statusLabel();

private slots:
    void on_findButton_clicked();

    void on_addFriendButton_clicked();

private:
    Ui::FindFriendWindow *ui;
};

#endif // FINDFRIENDWINDOW_H
