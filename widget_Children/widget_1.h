#ifndef WIDGET_1_H
#define WIDGET_1_H

#include <QApplication>
#include <QWidget>
#include <QDebug>
#include <QCloseEvent>
#include <QPainter>
//#include "ui_widget_1.h"

namespace Ui {
class widget_1;
}

class widget_1 : public QWidget
{
    Q_OBJECT

public:
    explicit widget_1(QWidget *parent = nullptr);
    ~widget_1();

private:
    Ui::widget_1 *ui;

private:
    void initUi();
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void CloseInfo();
};

#endif // WIDGET_1_H
