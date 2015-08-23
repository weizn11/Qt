#ifndef FINDGROUPWINDOW_H
#define FINDGROUPWINDOW_H

#include <QDialog>
#include "global.h"

namespace Ui {
class FindGroupWindow;
}

class FindGroupWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FindGroupWindow(QWidget *parent = 0);
    ~FindGroupWindow();
    void set_window_size(unsigned int wide,unsigned int high);

private slots:
    void on_findButton_clicked();

    void on_joinButton_clicked();

private:
    Ui::FindGroupWindow *ui;
};

#endif // FINDGROUPWINDOW_H
