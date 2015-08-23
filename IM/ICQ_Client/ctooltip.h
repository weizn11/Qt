#ifndef CTOOLTIP
#define CTOOLTIP

#include "global.h"
#include "friendstreeview.h"

class CToolTip : public QWidget
{
    Q_OBJECT
public:
    explicit CToolTip(QWidget *parent = 0);
    void showMessage(FRIEND_INFO_DATA friendInfoData, QPoint point);
signals:

public slots:

private:
    QLabel *labelIcon;
    QLabel *labelName;
    QLabel *labelInfo;

    QHBoxLayout *horLayout;
    QVBoxLayout *verlayout;

    QGroupBox *groupBox;

protected:
    void hoverEvent(QHoverEvent *);
};

#endif // CTOOLTIP

