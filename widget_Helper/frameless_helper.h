#ifndef FRAMELESS_HELPER_H
#define FRAMELESS_HELPER_H

#include <QWidget>
#include <QApplication>
#include <QObject>
#include <QPoint>
#include <QHash>
#include <QMouseEvent>
#include <QRubberBand>
#include <QHoverEvent>
#include <QRect>
#include <QDebug>

class FramelessHelperPrivate;
class CursorPosCalculator;
class WidgetData;

class FramelessHelper : public QObject
{
    Q_OBJECT

public:
    explicit FramelessHelper(QObject *parent = nullptr);
    ~FramelessHelper();

    void activateOn(QWidget *topLevelWidget);  // 激活窗体
    void removeFrom(QWidget *topLevelWidget);  // 移除窗体
    void setWidgetMovable(bool movable);  // 设置窗体移动
    void setWidgetResizable(bool resizable);  // 设置窗体缩放
    void setRubberBandOnMove(bool movable);  // 设置橡皮筋移动
    void setRubberBandOnResize(bool resizable);  // 设置橡皮筋缩放
    void setBorderWidth(int width);  // 设置边框的宽度
    bool widgetResizable();
    bool widgetMovable();
    bool rubberBandOnMove();
    bool rubberBandOnResisze();
    int borderWidth();

protected:
    // 事件过滤，进行移动、缩放等
    virtual bool eventFilter(QObject *obj, QEvent *event);

private:
    FramelessHelperPrivate *d;
};

/**************************** FramelessHelperPrivate******* 存储界面对应的数据集合，以及是否可移动、可缩放属性************************************/
class FramelessHelperPrivate
{
public:
    QHash<QWidget*, WidgetData*> widgetDataHash;
    bool WidgetMovable        : true;
    bool WidgetResizable      : true;
    bool RubberBandOnResize   : true;
    bool RubberBandOnMove     : true;
};

/******************************* CursorPosCalculator******* 计算鼠标是否位于左、上、右、下、左上角、左下角、右上角、右下角*******************************/
class CursorPosCalculator
{
public:
    explicit CursorPosCalculator();
    void reset();
    void recalculate(const QPoint &globalMousePos, const QRect &frameRect);

public:
    bool OnEdges              : true;
    bool OnLeftEdge           : true;
    bool OnRightEdge          : true;
    bool OnTopEdge            : true;
    bool OnBottomEdge         : true;
    bool OnTopLeftEdge        : true;
    bool OnBottomLeftEdge     : true;
    bool OnTopRightEdge       : true;
    bool OnBottomRightEdge    : true;

    static int m_nBorderWidth;
};

/*************************************** WidgetData********************* 更新鼠标样式、移动窗体、缩放窗体******************************************/
class WidgetData
{
public:
    explicit WidgetData(FramelessHelperPrivate *_d, QWidget *TopLevelWidget);
    ~WidgetData();
    QWidget* widget();
    // 处理鼠标事件-划过、按下、释放、移动
    void handleWidgetEvent(QEvent *event);
    // 更新橡皮筋状态
    void updateRubberBandStatus();

private:
    // 更新鼠标样式
    void updateCursorShape(const QPoint &gMousePos);
    // 重置窗体大小
    void resizeWidget(const QPoint &gMousePos);
    // 移动窗体
    void moveWidget(const QPoint &gMousePos);
    // 处理鼠标按下
    void handleMousePressEvent(QMouseEvent *event);
    // 处理鼠标释放
    void handleMouseReleaseEvent(QMouseEvent *event);
    // 处理鼠标移动
    void handleMouseMoveEvent(QMouseEvent *event);
    // 处理鼠标离开
    void handleLeaveEvent(QEvent *event);
    // 处理鼠标进入
    void handleHoverMoveEvent(QHoverEvent *event);

private:
    FramelessHelperPrivate *d;
    QRubberBand *RubberBand;
    QWidget *Widget;
    QPoint DragPos;
    CursorPosCalculator pressedMousePos;
    CursorPosCalculator moveMousePos;
    bool LeftButtonPressed;
    bool CursorShapeChanged;
    bool LeftButtonTitlePressed;
    Qt::WindowFlags windowFlags;
};

#endif // FRAMELESS_HELPER_H
