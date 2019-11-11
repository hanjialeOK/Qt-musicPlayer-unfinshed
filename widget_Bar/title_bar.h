#ifndef TITLE_BAR_H
#define TITLE_BAR_H

#include <QWidget>
#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QIcon>
#include <QDebug>
#include <QRect>
#include <QPainter>
#include <QFrame>
#include <QGuiApplication>
#include <QTimer>

class CCPushButton;

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar();

public:
    QLabel *IconLabel = nullptr;
    QLabel *TitleLabel = nullptr;

private:
    QFrame *frame = nullptr;
    QLabel *VlineLabel = nullptr;
    CCPushButton *ChangeSkinButton = nullptr;
    CCPushButton *MessageButton = nullptr;
    CCPushButton *SetupButton = nullptr;
    QPushButton *MinimizeButton = nullptr;
    QPushButton *MaximizeButton = nullptr;
    QPushButton *CloseButton = nullptr;

private:
    void initUi();
//    void mouseDoubleClickEvent(QMouseEvent *event);
    void updateMaximize();

protected:
    void focusInEvent(QFocusEvent *) override;
    void focusOutEvent(QFocusEvent *) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void on_clicked();

signals:
    void SetupButton_Clicked();
    void CloseButton_Clicked();
};

//********************CCPushButton*****************************CCPushButton*******************CCPushButton*****************************//

class CCPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CCPushButton(QWidget *parent = nullptr);

protected:
    //bool event(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    enum state { State_Normal, State_Hover };
    QTimer *timerN1 = nullptr;
    QTimer *timerN2 = nullptr;
    QPainterPath ellipse;
    state btnState;
    QPoint clickPos;
    int radius;
    int color;
    int cnt;
};

#endif // TITLE_BAR_H
