#include "widget_Children/widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    initUi();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initUi()
{
    setWindowFlags(Qt::FramelessWindowHint);

    resize(1130 - 250, 750 - 120);
//    setMinimumSize(0, 0);
//    setMaximumSize(1920 - 250, 1030 - 120);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(250, 250, 250));
    //pal.setColor(QPalette::Background, QColor(200, 200, 200));
    setAutoFillBackground(true);
    setPalette(pal);
}

void Widget::closeEvent(QCloseEvent *event)
{
    event->accept();
    qDebug()<<"主窗口退出...";
}
