#ifndef CREATEGROUP_H
#define CREATEGROUP_H

#include <QDialog>
#include "global.h"
#include "findfriendwindow.h"

namespace Ui {
class CreateGroup;
}

class CreateGroup : public QDialog
{
    Q_OBJECT

public:
    explicit CreateGroup(QWidget *parent = 0);
    void show();
    ~CreateGroup();

private slots:
    void on_createButton_clicked();

private:
    Ui::CreateGroup *ui;
};

#endif // CREATEGROUP_H
