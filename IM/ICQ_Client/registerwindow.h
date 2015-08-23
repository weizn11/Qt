#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QDialog>

namespace Ui {
class RegisterWindow;
}

class RegisterWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = 0);
    ~RegisterWindow();
protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);

private slots:
    void on_closeButton_clicked();
    void on_registerButton_clicked();

private:
    Ui::RegisterWindow *ui;

    QPixmap m_pixmapBg;
    QAction *m_AactionAboutQt;

    QPoint m_pointStart;
    QPoint m_pointPress;
};

#endif // REGISTERWINDOW_H
