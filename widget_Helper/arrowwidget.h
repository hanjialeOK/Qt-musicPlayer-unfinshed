#ifndef ARROWWIDGET_H
#define ARROWWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QKeyEvent>
#include <QDebug>
#include <QTimer>
#include <QPropertyAnimation>
#include <windows.h>

//*******************************ArrowWidget*****************************************ArrowWidget*******************************************//
#define SHADOW_WIDTH 10                 // 窗口阴影宽度;
#define TRIANGLE_WIDTH 15               // 小三角的宽度;
#define TRIANGLE_HEIGHT 10              // 小三角的高度;
#define BORDER_RADIUS 5                 // 窗口边角的弧度;

class ArrowWidget : public QWidget
{
    Q_OBJECT

public:
    ArrowWidget(QWidget *parent = nullptr);

    QLabel* textLabel = nullptr;
    void setStartPos(int startX); // 设置小三角起始位置;
    void setTriangleInfo(int width, int height); // 设置小三角宽和高;
    void setCenterWidget(QWidget* widget); // 设置中间区域widget;

protected:
    void paintEvent(QPaintEvent *) override;

private:
    int m_startX; // 小三角起始位置;
    int m_triangleWidth; // 小三角的宽度;
    int m_triangleHeight; // 小三角高度;
};

//****************************AddDirWidget**********************************AddDirWidget***************************************************//

#define SHADOW_WIDTH_1 10                 // 窗口阴影宽度;
#define TRIANGLE_WIDTH_1 15               // 小三角的宽度;
#define TRIANGLE_HEIGHT_1 10              // 小三角的高度;
#define BORDER_RADIUS_1 5                 // 窗口边角的弧度;

class AddDirWidget : public QWidget
{
    Q_OBJECT

public:
    AddDirWidget(QWidget *parent = nullptr);
    void setStartPos(int startX); // 设置小三角起始位置;
    void setTriangleInfo(int width, int height); // 设置小三角宽和高;

public:
    int X_offset, Y_offset;
    QLineEdit *DirName = nullptr;

private:
    int m_startX; // 小三角起始位置;
    int m_triangleWidth; // 小三角的宽度;
    int m_triangleHeight; // 小三角高度;
    int m_width, m_height;
    QLabel *CreateNewDirLabel = nullptr;
    QPushButton *ConfirmButton = nullptr;
    QPushButton *CancelButton = nullptr;
    QTimer *timer;

private:
    void initUi();
    void closeWidget();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool event(QEvent *event) override;

private slots:
    void changeState(const QString &str);
    void on_ConfirmButton_Clicked();
    void on_CancelButton_Clicked();

signals:
    void Confirmed(QString str);
    void highlightButtonIcon();
    void restoreButtonIcon();
};

#endif // ARROWWIDGET_H
