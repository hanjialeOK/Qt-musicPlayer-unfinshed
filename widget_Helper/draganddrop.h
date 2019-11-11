#ifndef DRAGANDDROP_H
#define DRAGANDDROP_H

#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QRect>
#include <QTimer>
#include <QPainter>
#include <QBoxLayout>

static const char * const move_xpm[] = {
    "24 28 3 1",
    ".        c None",
    "a        c #000000",
    "X        c #FFFFFF",
    "a.......................",
    "aa......................",
    "aXa.....................",
    "aXXa....................",
    "aXXXa...................",
    "aXXXXa..................",
    "aXXXXXa.................",
    "aXXXXXXa................",
    "aXXXXXXXa...............",
    "aXXXXXXXXa..............",
    "aXXXXXXXXXa.............",
    "aXXXXXXaaaaa............",
    "aXXXXXXa................",
    "aXXaaXXa................",
    "aXa..aXXa...............",
    "aa...aXXa...............",
    "a.....aXXa..............",
    "......aXXa..............",
    "...aXaXaXXaaXaXa........",
    "...XaXaaXXaXaXaX........",
    "...aX...aa....Xa........",
    "...Xa.........aX........",
    "...aX.........Xa........",
    "...Xa.........aX........",
    "...aX.........Xa........",
    "...XaXaXaXaXaXaX........",
    "...aXaXaXaXaXaXa........",
    "........................"};

static const char * const copy_xpm[] = {
"24 31 3 1",
".        c None",
"a        c #000000",
"X        c #FFFFFF",
"a.......................",
"aa......................",
"aXa.....................",
"aXXa....................",
"aXXXa...................",
"aXXXXa..................",
"aXXXXXa.................",
"aXXXXXXa................",
"aXXXXXXXa...............",
"aXXXXXXXXa..............",
"aXXXXXXXXXa.............",
"aXXXXXXaaaaa............",
"aXXXXXXa................",
"aXXaaXXa................",
"aXa..aXXa...............",
"aa...aXXa...............",
"a.....aXXa..............",
"......aXXa..............",
".......aXXa.............",
".......aXXa.............",
"........aa...aaaaaaaaaaa",
".............aXXXXXXXXXa",
".............aXXXXXXXXXa",
".............aXXXXaXXXXa",
".............aXXXXaXXXXa",
".............aXXaaaaaXXa",
".............aXXXXaXXXXa",
".............aXXXXaXXXXa",
".............aXXXXXXXXXa",
".............aXXXXXXXXXa",
".............aaaaaaaaaaa"};

//********************thumbnail**********************thumbnail*********************thumbnail*******************thumbnail****************//
#define RADIUS 17             //窗口边角的弧度
#define ELLIPSE_RADIUS 12     //内部小圆半径
#define RECT 10               //图标长/宽的一半
#define TEXT_LENGTH 100       //文字长度

class thumbnail : public QWidget
{
    Q_OBJECT
public:
    thumbnail(QWidget *parent = nullptr);
    void setupthumbnail(QIcon icon, QString str);
    void setIconSize(int size);

private:
    QPushButton *ImageButton = nullptr;
    QLabel *TextLabel = nullptr;

private:
    void initUi();
    void paintEvent(QPaintEvent *);
};

//************************MyTimer***********************MyTimer************************MyTimer**********************MyTimer*******************//

class MyTimer : public QTimer
{
    Q_OBJECT

public:
    explicit MyTimer(QObject *parent = nullptr) :
        QTimer(parent)
    {
        setTimerType(Qt::PreciseTimer);
    }

public slots:
    void getInterval(int value) {setInterval(value);}
    void beginTimer() {start(10);}
    void stopTimer() {stop();}
};

#endif // DRAGANDDROP_H
