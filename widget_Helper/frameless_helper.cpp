#include "frameless_helper.h"

/**************************************************************FramelessHelper***************************************************************/
FramelessHelper::FramelessHelper(QObject *parent) :
      QObject(parent),
      d(new FramelessHelperPrivate())
{
    d->WidgetMovable = true;
    d->WidgetResizable = true;
    d->RubberBandOnResize = false;
    d->RubberBandOnMove = false;
    qDebug()<<"FramelessHelper初始化...";
}

FramelessHelper::~FramelessHelper()
{
    QList<QWidget*> keys = d->widgetDataHash.keys();
    int size = keys.size();
    for(int i = 0; i < size; ++i){
        delete d->widgetDataHash.take(keys[i]);
    }
    delete d;
}

bool FramelessHelper::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()){
        case QEvent::MouseMove: break;
        case QEvent::HoverMove: break;
        case QEvent::MouseButtonPress: break;
        case QEvent::MouseButtonRelease: break;
        case QEvent::Leave:{
            WidgetData *data = d->widgetDataHash.value(static_cast<QWidget*>(obj));
            if(data){
            data->handleWidgetEvent(event);
            return true;
            }
            break;
        }
        default: return QObject::eventFilter(obj, event);
    }
    return QObject::eventFilter(obj, event);
}

void FramelessHelper::activateOn(QWidget *topLevelWidget)
{
    if(!d->widgetDataHash.contains(topLevelWidget)){
        WidgetData *data = new WidgetData(d, topLevelWidget);
        d->widgetDataHash.insert(topLevelWidget, data);

        topLevelWidget->installEventFilter(this);
        qDebug()<<"激活窗体...";
    }
}

void FramelessHelper::removeFrom(QWidget *topLevelWidget)
{
    WidgetData *data = d->widgetDataHash.take(topLevelWidget);
    if(data){
        topLevelWidget->removeEventFilter(this);
        delete data;
    }
}

void FramelessHelper::setRubberBandOnMove(bool movable)
{
    d->RubberBandOnMove = movable;
    QList<WidgetData*> list = d->widgetDataHash.values();
    foreach(WidgetData *data, list){
        data->updateRubberBandStatus();
    }
}

void FramelessHelper::setWidgetMovable(bool movable)
{
    d->WidgetMovable = movable;
}

void FramelessHelper::setWidgetResizable(bool resizable)
{
    d->WidgetResizable = resizable;
}

void FramelessHelper::setRubberBandOnResize(bool resizable)
{
    d->RubberBandOnResize = resizable;
    QList<WidgetData*> list = d->widgetDataHash.values();
    foreach(WidgetData *data, list){
        data->updateRubberBandStatus();
    }
}

void FramelessHelper::setBorderWidth(int width)
{
    if(width > 0){
        CursorPosCalculator::m_nBorderWidth = width;
    }
}

bool FramelessHelper::widgetMovable()
{
    return d->WidgetMovable;
}

bool FramelessHelper::widgetResizable()
{
    return d->WidgetResizable;
}

bool FramelessHelper::rubberBandOnMove()
{
    return d->RubberBandOnMove;
}

bool FramelessHelper::rubberBandOnResisze()
{
    return d->RubberBandOnResize;
}

int FramelessHelper::borderWidth()
{
    return CursorPosCalculator::m_nBorderWidth;
}

/********************************************************* CursorPosCalculator ********************************************************************/
int CursorPosCalculator::m_nBorderWidth = 5;

CursorPosCalculator::CursorPosCalculator()
{
    reset();
    qDebug()<<"CursorPosCalculator初始化...";
}

void CursorPosCalculator::reset()
{
    OnEdges = false;
    OnLeftEdge = false;
    OnRightEdge = false;
    OnTopEdge = false;
    OnBottomEdge = false;
    OnTopLeftEdge = false;
    OnBottomLeftEdge = false;
    OnTopRightEdge  = false;
    OnBottomRightEdge = false;
}

void CursorPosCalculator::recalculate(const QPoint &gMousePos, const QRect &frameRect)
{
    int globalMouseX = gMousePos.x();
    int globalMouseY = gMousePos.y();

    int frameX = frameRect.x();
    int frameY = frameRect.y();

    int frameWidth = frameRect.width();
    int frameHeight = frameRect.height();

    OnLeftEdge = (globalMouseX >= frameX && globalMouseX <= frameX + m_nBorderWidth );

    OnRightEdge = (globalMouseX >= frameX + frameWidth - m_nBorderWidth && globalMouseX <= frameX + frameWidth);

    OnTopEdge = (globalMouseY >= frameY && globalMouseY <= frameY + m_nBorderWidth );

    OnBottomEdge = (globalMouseY >= frameY + frameHeight - m_nBorderWidth && globalMouseY <= frameY + frameHeight);

    OnTopLeftEdge = OnTopEdge && OnLeftEdge;
    OnBottomLeftEdge = OnBottomEdge && OnLeftEdge;
    OnTopRightEdge = OnTopEdge && OnRightEdge;
    OnBottomRightEdge = OnBottomEdge && OnRightEdge;

    OnEdges = OnLeftEdge || OnRightEdge || OnTopEdge || OnBottomEdge;
}

/************************************************************ WidgetData **************************************************************************/
WidgetData::WidgetData(FramelessHelperPrivate *_d, QWidget *TopLevelWidget)
{
    d = _d;
    Widget = TopLevelWidget;
    LeftButtonPressed = false;
    CursorShapeChanged = false;
    LeftButtonTitlePressed = false;
    RubberBand = nullptr;

    windowFlags = Widget->windowFlags();
    Widget->setMouseTracking(true);
    Widget->setAttribute(Qt::WA_Hover, true);

    updateRubberBandStatus();
    qDebug()<<"WidgetData初始化...";
}

WidgetData::~WidgetData()
{
    Widget->setMouseTracking(false);
    Widget->setWindowFlags(windowFlags);
    Widget->setAttribute(Qt::WA_Hover, false);

    delete RubberBand;
    RubberBand = nullptr;
}

QWidget* WidgetData::widget()
{
    return Widget;
}

void WidgetData::handleWidgetEvent(QEvent *event)
{
    switch(event->type()){
    case QEvent::MouseButtonPress:
        handleMousePressEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseButtonRelease:
        handleMouseReleaseEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseMove:
        handleMouseMoveEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::Leave:
        handleLeaveEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::HoverMove:
        handleHoverMoveEvent(static_cast<QHoverEvent*>(event));
        break;
    default:
        break;
    }
}

void WidgetData::updateRubberBandStatus()
{
    if(d->RubberBandOnMove || d->RubberBandOnResize){
        if(RubberBand == nullptr)
            RubberBand = new QRubberBand(QRubberBand::Rectangle);
    }
    else{
        delete RubberBand;
        RubberBand = nullptr;
    }
}

void WidgetData::updateCursorShape(const QPoint &gMousePos)
{
    if(Widget->isFullScreen() || Widget->isMaximized()){
        if(CursorShapeChanged){
            Widget->unsetCursor();
        }
        return;
    }
    qDebug()<<"更换鼠标样式...";

    moveMousePos.recalculate(gMousePos, Widget->frameGeometry());

    if(moveMousePos.OnTopLeftEdge || moveMousePos.OnBottomRightEdge){
        Widget->setCursor(Qt::SizeFDiagCursor);
        CursorShapeChanged = true;
    }
    else if(moveMousePos.OnTopRightEdge || moveMousePos.OnBottomLeftEdge){
        Widget->setCursor(Qt::SizeBDiagCursor);
        CursorShapeChanged = true;
    }
    else if(moveMousePos.OnLeftEdge || moveMousePos.OnRightEdge){
        Widget->setCursor( Qt::SizeHorCursor );
        CursorShapeChanged = true;
    }
    else if(moveMousePos.OnTopEdge || moveMousePos.OnBottomEdge){
        Widget->setCursor( Qt::SizeVerCursor );
        CursorShapeChanged = true;
    }
    else{
        if(CursorShapeChanged){
            Widget->unsetCursor();
            CursorShapeChanged = false;
        }
    }
}

void WidgetData::resizeWidget(const QPoint &gMousePos)
{
    QRect origRect;

    if(d->RubberBandOnResize)
        origRect = RubberBand->frameGeometry();
    else
        origRect = Widget->frameGeometry();

    int left = origRect.left();
    int top = origRect.top();
    int right = origRect.right();
    int bottom = origRect.bottom();
    origRect.getCoords(&left, &top, &right, &bottom);

    int minWidth = Widget->minimumWidth();
    int minHeight = Widget->minimumHeight();

    if (pressedMousePos.OnTopLeftEdge){
        left = gMousePos.x();
        top = gMousePos.y();
    }
    else if (pressedMousePos.OnBottomLeftEdge){
        left = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (pressedMousePos.OnTopRightEdge){
        right = gMousePos.x();
        top = gMousePos.y();
    }
    else if (pressedMousePos.OnBottomRightEdge){
        right = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (pressedMousePos.OnLeftEdge){
        left = gMousePos.x();
    }
    else if (pressedMousePos.OnRightEdge){
        right = gMousePos.x();
    }
    else if (pressedMousePos.OnTopEdge){
        top = gMousePos.y();
    }
    else if (pressedMousePos.OnBottomEdge){
        bottom = gMousePos.y();
    }

    QRect newRect(QPoint(left, top), QPoint(right, bottom));

    if (newRect.isValid())
    {
        if (minWidth > newRect.width())
        {
            if (left != origRect.left())
                newRect.setLeft(origRect.left());
            else
                newRect.setRight(origRect.right());
        }
        if (minHeight > newRect.height())
        {
            if (top != origRect.top())
                newRect.setTop(origRect.top());
            else
                newRect.setBottom(origRect.bottom());
        }

        if (d->RubberBandOnResize)
        {
            RubberBand->setGeometry(newRect);
        }
        else
        {
            Widget->setGeometry(newRect);
        }
    }
}

void WidgetData::moveWidget(const QPoint& gMousePos)
{
    if (d->RubberBandOnMove)
    {
        RubberBand->move(gMousePos - DragPos);
    }
    else
    {
        Widget->move(gMousePos - DragPos);
    }
}

void WidgetData::handleMousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        LeftButtonPressed = true;
        LeftButtonTitlePressed = event->pos().y();

        QRect frameRect = Widget->frameGeometry();
        pressedMousePos.recalculate(event->globalPos(), frameRect);

        DragPos = event->globalPos() - frameRect.topLeft();

        if(pressedMousePos.OnEdges){
            if (d->RubberBandOnResize){
                RubberBand->setGeometry(frameRect);
                RubberBand->show();
            }
        }
        else if (d->RubberBandOnMove){
            RubberBand->setGeometry(frameRect);
            RubberBand->show();
        }
    }
}

void WidgetData::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        LeftButtonPressed = false;
        LeftButtonTitlePressed = false;
        pressedMousePos.reset();
        if (RubberBand && RubberBand->isVisible()){
            RubberBand->hide();
            Widget->setGeometry(RubberBand->geometry());
        }
    }
}

void WidgetData::handleMouseMoveEvent(QMouseEvent *event)
{
    if (LeftButtonPressed){
        if (d->WidgetResizable && pressedMousePos.OnEdges){
            resizeWidget(event->globalPos());
        }
        else if (d->WidgetMovable && LeftButtonTitlePressed){
            moveWidget(event->globalPos());
        }
    }
    else if (d->WidgetResizable){
        updateCursorShape(event->globalPos());
    }
}

void WidgetData::handleLeaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    if (!LeftButtonPressed){
        Widget->unsetCursor();
    }
}

void WidgetData::handleHoverMoveEvent(QHoverEvent *event)
{
    if (d->WidgetResizable){
        updateCursorShape(Widget->mapToGlobal(event->pos()));
    }
}
