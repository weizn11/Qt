#include "filerecvform.h"
#include "ui_filerecvform.h"
#include "commun/commun.h"

extern ServerConn gl_Server_Conn_Info;

FileRecvForm::FileRecvForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileRecvForm)
{
    ui->setupUi(this);

    ui->recvButton->setStyleSheet("QPushButton{background-color:rgba(255,255,255,100);}");
    ui->rejectButton->setStyleSheet("QPushButton{background-color:rgba(255,255,255,100);}");
}

void FileRecvForm::set_recv_file_info(FILE_TRAN_DATA fileTranData)
{
    QString qstr;

    qstr.sprintf("%s(%d KB)",fileTranData.fileName,fileTranData.fileSize/1024);
    ui->label->setText(qstr);
    ui->progressBar->setRange(0,fileTranData.fileSize);
    ui->progressBar->setValue(0);
    this->fileTranData=fileTranData;

    return;
}

void FileRecvForm::set_label_text(char *str)
{
    ui->label->setText(str);
    return;
}

void FileRecvForm::set_progress_range(int start, int end)
{
    ui->progressBar->setRange(start,end);
    return;
}

void FileRecvForm::set_progress_value(int value)
{
    ui->progressBar->setValue(value);
    return;
}

void FileRecvForm::ex_show()
{
    ui->recvButton->show();
    ui->rejectButton->show();
    this->show();
    return;
}

FileRecvForm::~FileRecvForm()
{
    delete ui;
}

void FileRecvForm::on_rejectButton_clicked()
{
    char str[ID_MAXIMUM_SIZE];

    fileTranData.auth=-1;
    memset(str,NULL,sizeof(str));
    strcat(str,fileTranData.fromNum);
    memset(fileTranData.fromNum,NULL,sizeof(fileTranData.fromNum));
    strcat(fileTranData.fromNum,fileTranData.toNum);
    memset(fileTranData.toNum,NULL,sizeof(fileTranData.toNum));
    strcat(fileTranData.toNum,str);

    gl_Server_Conn_Info.ex_send((char *)&fileTranData,sizeof(fileTranData),10);

    this->close();
}

void FileRecvForm::on_recvButton_clicked()
{
    QString qstrFilePath;
    char *pFilePath=NULL;
    char str[ID_MAXIMUM_SIZE];
    FILE_TRAN_LIST fileTranList;
    FILE_TRAN_DATA fileTranData;
    P2PFileRecvThread *pP2PFileRecvThread=NULL;

    memset(&fileTranList,NULL,sizeof(fileTranList));
    memset(&fileTranData,NULL,sizeof(fileTranData));

    fileTranList.fileTranData=this->fileTranData;
    qstrFilePath=QFileDialog::getSaveFileName(this,"保存路径",this->fileTranData.fileName,"*.*");
    if(qstrFilePath.isEmpty())
        return;
    pFilePath=QString_to_char(qstrFilePath);
    qDebug() <<pFilePath;
    pFilePath=UTF8ToANSI(pFilePath);
    fileTranList.pFile=fopen(pFilePath,"wb");
    if(fileTranList.pFile==NULL)
    {
        ui->label->setText("创建新文件失败！");\
        return;
    }
    strcat(fileTranList.filePath,pFilePath);
    gl_Server_Conn_Info.vectorFileRecvList.push_back(fileTranList);

    fileTranData=fileTranList.fileTranData;
    fileTranData.auth=1;
    memset(str,NULL,sizeof(str));
    strcat(str,fileTranData.fromNum);
    memset(fileTranData.fromNum,NULL,sizeof(fileTranData.fromNum));
    strcat(fileTranData.fromNum,fileTranData.toNum);
    memset(fileTranData.toNum,NULL,sizeof(fileTranData.toNum));
    strcat(fileTranData.toNum,str);
    ui->recvButton->hide();
    ui->rejectButton->hide();
    ui->label->setText("正在建立连接...");
    gl_Server_Conn_Info.ex_send((char *)&fileTranData,sizeof(fileTranData),10);
    pP2PFileRecvThread=new P2PFileRecvThread(fileTranData);
    pP2PFileRecvThread->start();

    return;
}






