#ifndef FILERECVFORM_H
#define FILERECVFORM_H

#include <QWidget>
#include "global.h"

namespace Ui {
class FileRecvForm;
}

class FileRecvForm : public QWidget
{
    Q_OBJECT

public:
    explicit FileRecvForm(QWidget *parent = 0);
    void set_recv_file_info(FILE_TRAN_DATA fileTranData);
    void set_label_text(char *str);
    void set_progress_range(int start,int end);
    void set_progress_value(int value);
    void ex_show();
    ~FileRecvForm();

private slots:
    void on_rejectButton_clicked();
    void on_recvButton_clicked();

public:
    FILE_TRAN_DATA fileTranData;
private:
    Ui::FileRecvForm *ui;
};

#endif // FILERECVFORM_H
