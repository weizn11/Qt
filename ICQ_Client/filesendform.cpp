#include "filesendform.h"
#include "ui_filesendform.h"

FileSendForm::FileSendForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileSendForm)
{
    ui->setupUi(this);
}

void FileSendForm::set_label_text(char *str)
{
    ui->label->setText(str);
    ui->label->repaint();
    return;
}

void FileSendForm::set_progress_range(int start, int end)
{
    ui->progressBar->setRange(start,end);
    return;
}

void FileSendForm::set_progress_value(int value)
{
    ui->progressBar->setValue(value);
    return;
}

FileSendForm::~FileSendForm()
{
    delete ui;
}
