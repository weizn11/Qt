#ifndef FILESENDFORM_H
#define FILESENDFORM_H

#include <QWidget>

namespace Ui {
class FileSendForm;
}

class FileSendForm : public QWidget
{
    Q_OBJECT

public:
    explicit FileSendForm(QWidget *parent = 0);
    void set_label_text(char *str);
    void set_progress_range(int start,int end);
    void set_progress_value(int value);
    ~FileSendForm();

private:
    Ui::FileSendForm *ui;
};

#endif // FILESENDFORM_H
