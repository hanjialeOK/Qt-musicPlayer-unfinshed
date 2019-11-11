#ifndef CFRAMELESSWINDOW_H
#define CFRAMELESSWINDOW_H
#include "qsystemdetection.h"
#include <QObject>
#include <QMainWindow>

#include <QWidget>
#include <QList>
#include <QMargins>
#include <QRect>
#include <QDebug>
class CFramelessWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit CFramelessWindow(QWidget *parent = nullptr);

public:
    //设置是否可以通过鼠标调整窗口大小
    void setResizeable(bool resizeable = true);
    bool isResizeable(){return m_bResizeable;}

    //设置可调整大小区域的宽度，在此区域内，可以使用鼠标调整窗口大小
    void setResizeableAreaWidth(int width = 5);

protected:
    //设置一个标题栏widget，此widget会被当做标题栏对待
    void setTitleBar(QWidget* titlebar);

    //在标题栏控件内，也可以有子控件如标签控件“label1”，此label1遮盖了标题栏，导致不能通过label1拖动窗口
    //要解决此问题，使用addIgnoreWidget(label1)
    void addIgnoreWidget(QWidget* widget);

    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private slots:
    void onTitleBarDestroyed();

public:
    void setContentsMargins(const QMargins &margins);
    void setContentsMargins(int left, int top, int right, int bottom);
    QMargins contentsMargins() const;
    QRect contentsRect() const;
    void getContentsMargins(int *left, int *top, int *right, int *bottom) const;
public slots:
    void showFullScreen();
private:
    QWidget* m_titlebar;
    QList<QWidget*> m_whiteList;
    int m_borderWidth;

    QMargins m_margins;
    QMargins m_frames;
    bool m_bJustMaximized;

    bool m_bResizeable;
    bool pressed = false;
};

#endif // CFRAMELESSWINDOW_H
