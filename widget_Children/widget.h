#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QCloseEvent>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

private:
    void initUi();
    void closeEvent(QCloseEvent *event = nullptr);
//    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // WIDGET_H
