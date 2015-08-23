#ifndef ONESELFINFOWINDOW_H
#define ONESELFINFOWINDOW_H

#include <QDialog>
#include "global.h"
#include "commun/commun.h"

namespace Ui {
class OneselfInfoWindow;
}

class OneselfInfoWindow : public QDialog
{
    Q_OBJECT

public:
    explicit OneselfInfoWindow(QWidget *parent = 0);
    ~OneselfInfoWindow();
    void ex_show();
    void ex_show(FRIEND_INFO_DATA friendInfoData);
protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
private slots:
    void on_applyButton_clicked();

    void on_uploadIconButton_clicked();

private:
    Ui::OneselfInfoWindow *ui;

    QPixmap m_pixmapBg;
    QAction *m_AactionAboutQt;
    QPoint m_pointStart;
    QPoint m_pointPress;
};

#endif // ONESELFINFOWINDOW_H
