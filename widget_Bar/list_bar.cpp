#include "widget_Bar/list_bar.h"

//*********************ItemContentsWidget***********************ItemContentsWidget**************************ItemContentsWidget*******//

ItemContentsWidget::ItemContentsWidget(QWidget *parent)  :
    QWidget(parent)
{
    initUi();
}

void ItemContentsWidget::initUi()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    resize(250, 40);
    ImageButton = new QPushButton(this);
    ImageButton->setFixedSize(30, 30);
    ImageButton->setIconSize(QSize(25, 25));
    ImageButton->setFlat(true);
    ImageButton->setFocusPolicy(Qt::NoFocus);
    ImageButton->setAttribute(Qt::WA_TransparentForMouseEvents, true);  //鼠标穿透效果

    TextLabel = new QLabel(this);
    TextLabel->setFixedSize(150, 26);
    TextLabel->setFocusPolicy(Qt::NoFocus);
    TextLabel->setFont(QFont("Microsoft YaHei", 9, QFont::Normal));
    TextLabel->setStyleSheet("QLabel{color:rgba(245, 66, 66, 200);}");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(ImageButton);
    layout->setSpacing(3);
    layout->addWidget(TextLabel);
    layout->addStretch();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

void ItemContentsWidget::setupWidget(const QIcon &icon, const QIcon &icon_hover, const QString &str)
{
    ImageButton->setIcon(icon);
    TextLabel->setText(str);
    Img = icon;
    Img_hover = icon_hover;
}

void ItemContentsWidget::setIconSize(int size)
{
    ImageButton->setIconSize(QSize(size, size));
}

void ItemContentsWidget::isItemSelected(bool SelectionMsg)
{
    if(isSelected && !SelectionMsg){
        ImageButton->setIcon(Img);
        TextLabel->setStyleSheet("QLabel{color:rgba(245, 66, 66, 200);}");
    }
    else if(!isSelected && SelectionMsg){
        ImageButton->setIcon(Img_hover);
        TextLabel->setStyleSheet("QLabel{color:rgba(66, 133, 245, 255);}");
    }
    isSelected = SelectionMsg;
}

bool ItemContentsWidget::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Enter:
        if(!isSelected){
            ImageButton->setIcon(Img_hover);
            TextLabel->setStyleSheet("QLabel{color:rgba(66, 133, 245, 255);}");
        }
        break;
    case QEvent::Leave:
        if(!isSelected){
            ImageButton->setIcon(Img);
            TextLabel->setStyleSheet("QLabel{color:rgba(245, 66, 66, 200);}");
        }
        break;
    default:
        break;
    }
    return QWidget::event(event);
}
//*****************************CustomListWidget**************************CustomListWidget***************************CustomListWidget*****//

CustomListWidget::CustomListWidget(QWidget *parent) :
    QListWidget(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(false);
    setDefaultDropAction(Qt::MoveAction);

}

void CustomListWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton){
        startPos = event->pos();
    }
}

void CustomListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QListWidget::mouseReleaseEvent(event);
    itemAt(event->pos())->setSelected(true);
}

void CustomListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton){
        if((event->pos() - startPos).manhattanLength() < QApplication::startDragDistance()) return;

        QListWidgetItem *theDragItem = itemAt(startPos);
        theDragItem->setSelected(true);
        theDragRow = row(theDragItem);

        ItemContentsWidget *theDragWidget = qobject_cast<ItemContentsWidget *>(itemWidget(theDragItem));

        QString text = theDragWidget->TextLabel->text();
        QIcon icon = theDragWidget->ImageButton->icon();

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << text << icon;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData(myMimeType(), itemData);

        thumbnail *DragImage = new thumbnail(this);
        DragImage->setupthumbnail(icon, text);
        QPixmap pixmap = DragImage->grab();

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));

        if(drag->exec(Qt::MoveAction) == Qt::MoveAction){
        }
    }
}

void CustomListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    CustomListWidget *source = qobject_cast<CustomListWidget *>(event->source());
    if (source && source == this) {
        //event->setDropAction(Qt::MoveAction);
        IsDraging = true;
        event->accept();
    }
}

void CustomListWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    IsDraging = false;
    thehighlightedRow = -2;
    update(thehighlightedRect);  //仅刷新离开时所在行
    event->accept();
}

void CustomListWidget::dragMoveEvent(QDragMoveEvent *event)
{   
    CustomListWidget *source = qobject_cast<CustomListWidget *>(event->source());
    if (source && source == this) {

        if(event->pos().y() >= offset()){

            oldhighlightedRect = thehighlightedRect;
            thehighlightedRect = targetRect(event->pos());
            thehighlightedRow = row(itemAt(event->pos() - QPoint(0, offset())));

            if(oldhighlightedRect != thehighlightedRect){
                update(oldhighlightedRect);
                update(thehighlightedRect);
            }else
                update(thehighlightedRect);
        }else{
            thehighlightedRow = -1;
            update(QRect(0, 0, width(), 80));
        }

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void CustomListWidget::dropEvent(QDropEvent *event)
{
    QListWidget::dropEvent(event);

    CustomListWidget *source = qobject_cast<CustomListWidget *>(event->source());
    if (source && source == this){

        IsDraging = false;
        thehighlightedRow = -2;

        update(thehighlightedRect);

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

//void CustomListWidget::startDrag(Qt::DropActions)
//{
//    QMimeData *mimeData = new QMimeData;
//    theDragItem = currentItem();
//    emit rowCount(count() - 1);  //通知delegate当前总行数
//    emit theDragRow(row(currentItem()));  //通知delegate当前拖拽行
//    theDragWidget = qobject_cast<ItemContentsWidget *>(itemWidget(theDragItem));

//    mimeData->setText(theDragWidget->TextLabel->text());
//    mimeData->setImageData(theDragWidget->ImageButton->icon());

//    thumbnail *DragImage = new thumbnail(this);
//    DragImage->setupthumbnail(theDragWidget->ImageButton->icon(), theDragWidget->TextLabel->text());
//    QPixmap Pixmap = DragImage->grab();

//    QDrag *drag = new QDrag(this);
//    drag->setMimeData(mimeData);
//    drag->setPixmap(Pixmap);
//    drag->setHotSpot(QPoint(Pixmap.width()/2, Pixmap.height()/2));

//    if(drag->exec(Qt::MoveAction) == Qt::MoveAction){
//    }
//}

const QRect CustomListWidget::targetRect(const QPoint &position) const
{
    return QRect(0, (position.y() - offset()) / 40 * 40, width(), 2 * 40);
}

//*********************CustomItemDelegate***********************CustomItemDelegate**************************CustomItemDelegate*******//

CustomItemDelegate::CustomItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void CustomItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    CustomListWidget *dragWidget = qobject_cast<CustomListWidget *>(option.styleObject);
    bool isDraging = dragWidget->isDraging();

    QRect rect = option.rect;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    if (option.state & QStyle::State_Selected){
    }

    if (option.state & QStyle::State_MouseOver){
    }

    if(isDraging){
        int theDragRow = dragWidget->dragRow();
        int UpRow = dragWidget->highlightedRow();
        int DownRow = UpRow + 1;
        int rowCount = dragWidget->count() - 1;

        if(index.row() == UpRow && index.row() != theDragRow - 1){
            painter->setBrush(QColor(66, 133, 244));

            if(UpRow == rowCount){
                //到达尾部,三角形向上移动一个WIDTH的距离,以使分隔符宽度*2
                QPolygon trianglePolygon_bottomLeft;
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (POLYGON + WIDTH) + 1 - WIDTH);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1 - WIDTH);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x() + POLYGON, rect.bottomLeft().y() - WIDTH + 1 - WIDTH);

                QPolygon trianglePolygon_bottomRight;
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x(), rect.bottomRight().y() - (POLYGON + WIDTH) + 1 - WIDTH);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x(), rect.bottomRight().y() - WIDTH + 1 - WIDTH);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() - POLYGON, rect.bottomRight().y() - WIDTH + 1 - WIDTH);

                painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - 2 * WIDTH + 1, rect.width(), 2 * WIDTH);  //rect
                painter->drawPolygon(trianglePolygon_bottomLeft);
                painter->drawPolygon(trianglePolygon_bottomRight);
            }
            else {
                //正常情况,组成上半部分(+1是根据实际情况修正)
                QPolygon trianglePolygon_bottomLeft;
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (POLYGON + WIDTH) + 1);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x() + POLYGON, rect.bottomLeft().y() - WIDTH + 1);

                QPolygon trianglePolygon_bottomRight;
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x(), rect.bottomRight().y() - (POLYGON + WIDTH) + 1);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x(), rect.bottomRight().y() - WIDTH + 1);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() - POLYGON, rect.bottomRight().y() - WIDTH + 1);

                painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1, rect.width(), WIDTH);  //rect
                painter->drawPolygon(trianglePolygon_bottomLeft);
                painter->drawPolygon(trianglePolygon_bottomRight);
            }
        }
        else if(index.row() == DownRow && index.row() != theDragRow + 1){
            painter->setBrush(QColor(66, 133, 244));

            if(DownRow == 0){
                //到达头部,三角形向下移动一个WIDTH的距离,以使分隔符宽度*2
                QPolygon trianglePolygon_topLeft;
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + (POLYGON + WIDTH) + WIDTH);
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + WIDTH + WIDTH);
                trianglePolygon_topLeft << QPoint(rect.topLeft().x() + POLYGON, rect.topLeft().y() + WIDTH + WIDTH);

                QPolygon trianglePolygon_topRight;
                trianglePolygon_topRight << QPoint(rect.topRight().x(), rect.topRight().y() + (POLYGON + WIDTH) + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x(), rect.topRight().y() + WIDTH + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x() - POLYGON, rect.topRight().y() + WIDTH + WIDTH);

                painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), rect.width(), 2 * WIDTH);  //rect
                painter->drawPolygon(trianglePolygon_topLeft);
                painter->drawPolygon(trianglePolygon_topRight);
            }
            else {
                //正常情况
                QPolygon trianglePolygon_topLeft;
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + (POLYGON + WIDTH));
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + WIDTH);
                trianglePolygon_topLeft << QPoint(rect.topLeft().x() + POLYGON, rect.topLeft().y() + WIDTH);

                QPolygon trianglePolygon_topRight;
                trianglePolygon_topRight << QPoint(rect.topRight().x(), rect.topRight().y() + (POLYGON + WIDTH));
                trianglePolygon_topRight << QPoint(rect.topRight().x(), rect.topRight().y() + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x() - POLYGON, rect.topRight().y() + WIDTH);

                painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), rect.width(), WIDTH);  //rect
                painter->drawPolygon(trianglePolygon_topLeft);
                painter->drawPolygon(trianglePolygon_topRight);
            }
        }
//        else{
//            painter->setBrush(QColor(245, 245, 247));
//            painter->drawRect(rect);
//        }

        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    QStyledItemDelegate::paint(painter, option, index);

}

//**************************MyListWidget***************************MyListWidget***************************MyListWidget*************************//

MyListWidget::MyListWidget(QWidget *parent) :
    QListWidget(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(false);
    setDefaultDropAction(Qt::MoveAction);
    //setMouseTracking(true);

    autoScrollTimer = new MyTimer();
    //autoScrollTimer->setInterval(10);
    QThread *timerThread = new QThread(this);
    autoScrollTimer->moveToThread(timerThread);
    connect(this, &MyListWidget::beginDrag, autoScrollTimer, &MyTimer::beginTimer);
    connect(this, &MyListWidget::endDrag, autoScrollTimer, &MyTimer::stopTimer);
    connect(this, &MyListWidget::Interval, autoScrollTimer, &MyTimer::getInterval);
    connect(autoScrollTimer, &MyTimer::timeout, this, &MyListWidget::doAutoScroll);
    connect(timerThread, &QThread::finished, autoScrollTimer, &MyTimer::deleteLater);
    timerThread->start();
}

void MyListWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton){
        startPos = event->pos();
    }
}

void MyListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QListWidgetItem *item = itemAt(event->pos());

    if(event->button() == Qt::LeftButton){
        //item->setSelected(true);
        setCurrentItem(item);
        emit itemPressed(item);  //手动发出item被选中信号
        update(selectedRect(event->pos()));
    }

    if(event->button() == Qt::RightButton){
        theRectByMenu = selectedRect(event->pos());
        theRowByMenu =row(item);
        update(theRectByMenu);
        emit ContextMenuClicked(row(item));  //发出菜单信号
    }
}

void MyListWidget::mouseMoveEvent(QMouseEvent *event)
{
    //QListWidget::mouseMoveEvent(event);  //不注释的话会导致拖拽即选中
    if(event->buttons() & Qt::LeftButton){
        if((event->pos() - startPos).manhattanLength() < QApplication::startDragDistance()) return;

        QListWidgetItem *theDragItem = itemAt(startPos);

        theDragRow = row(theDragItem);
        theSelectedRow = row(currentItem());

        update(QRect(0, 40 * theDragRow, width(), 40));

        QString text = theDragItem->text();
        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << text;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData(myMimeType(), itemData);

        thumbnail *DragImage = new thumbnail(this);
        DragImage->setupthumbnail(QIcon(":/listBar_Icon/1_hover.png"), text);
        DragImage->setIconSize(18);  //default:20
        QPixmap pixmap = DragImage->grab();

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
        connect(drag, &QDrag::actionChanged, [=](Qt::DropAction action){
            if(action == Qt::IgnoreAction) IsDropOutside = true;
            else IsDropOutside = false;
        });
        connect(drag, &QDrag::destroyed, this, &MyListWidget::updateItemStyle);

        drag->setDragCursor(QPixmap(const_cast<const char **>(move_xpm)), Qt::MoveAction);
        //drag->setDragCursor(QPixmap((const char **)move_xpm), Qt::IgnoreAction);

        if(drag->exec(Qt::MoveAction) == Qt::MoveAction){
            if(row(currentItem()) != row(theDragItem)) delete takeItem(row(theDragItem));
        }
    }
}

void MyListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    MyListWidget *source = qobject_cast<MyListWidget *>(event->source());
    if (source && source == this) {
        IsDraging = true;
        emit beginDrag();
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MyListWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
//this ensures DropIndicator to be disappear while dragleaving
    IsDraging = false;
    //emit endDrag();
    //timeCount = 0;
    thehighlightedRow = -2;

    if(!thehighlightedRect.contains(QRect(0, 40 * theDragRow, width(), 40))){
        update(thehighlightedRect);
    }

    //update(thehighlightedRect);

    theInsertRow = -1;
    event->accept();
}

void MyListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    MyListWidget *source = qobject_cast<MyListWidget *>(event->source());
    if (source && source == this) {

        oldhighlightedRect = thehighlightedRect;
        thehighlightedRect = targetRect(event->pos());

        if(event->pos().y() >= offset()){

            thehighlightedRow = row(itemAt(event->pos() - QPoint(0, offset())));

            if(oldhighlightedRect != thehighlightedRect){
                update(oldhighlightedRect);
                update(thehighlightedRect);
            }else
                update(thehighlightedRect);

            theInsertRow = row(itemAt(event->pos() - QPoint(0, offset()))) + 1;
        }else{
            thehighlightedRow = -1;
            update(QRect(0, 0, width(), 80));  //仅刷新第一行
            theInsertRow = 0;
        }

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MyListWidget::dropEvent(QDropEvent *event)
{
    MyListWidget *source = qobject_cast<MyListWidget *>(event->source());
    if (source && source == this){

        IsDraging = false;
        emit endDrag();
        timeCount = 0;

        thehighlightedRow = -2;
        update(thehighlightedRect);

        if(theInsertRow == theDragRow || theInsertRow == theDragRow + 1 || theInsertRow < 0){
            //theDragRow = -1;
            return;
        }

        if(theSelectedRow == theDragRow){
            QListWidget::dropEvent(event);
            //theDragRow = -1;

            return;
        }

        QString text;
        QByteArray itemData = event->mimeData()->data(myMimeType());
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        dataStream >> text;

        insertItem(theInsertRow, text);

//        if(insertRow < row(theDragItem)){
//            emit theInsertRow(insertRow);
//            update(QRect(0, 40 * (insertRow), width(), 40));
//        }else if(insertRow > row(theDragItem)){
//            emit theInsertRow(insertRow - 1);
//            update(QRect(0, 40 * (insertRow - 1), width(), 40));
//        }

        //theDragRow = -1;
        IsDropOutside = false;

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

//响应QScrollBar的Wheel()信号
void MyListWidget::HandleScrollBarWheeled()
{
    QPoint pos = mapFromGlobal(QCursor::pos());

    oldWheelRow = theWheelRow;
    theWheelRow = row(itemAt(pos));

    if(oldWheelRow != theWheelRow){
        update(0, 40 * oldWheelRow, width(), 40);
        update(0, 40 * theWheelRow, width(), 40);
    }
}

//响应子线程中QTimer的timeout()信号
void MyListWidget::doAutoScroll()
{
    QPoint pos = mapFromGlobal(QCursor::pos());
    if(pos.x() < 0 || pos.x() > width()){
        timeCount = 0;
        return;
    }
    int posYToParent = mapToParent(pos).y() - scrollBarValue;
    if(posYToParent < 0 || posYToParent > limitedHeight){
        timeCount = 0;
        return;
    }
    int step = 1;

    if(posYToParent >= limitedHeight - scrollMargin && scrollBarValue < scrollBarRange){
        if(timeCount >= timeLimit){
            emit Interval((limitedHeight - posYToParent + 5) / 3);
            emit exceptedValue(scrollBarValue + step);
            if(pos.y() >= offset()){
                thehighlightedRow = row(itemAt(pos - QPoint(0, offset())));

                oldhighlightedRect = thehighlightedRect;
                thehighlightedRect = targetRect(pos);

                if(oldhighlightedRect != thehighlightedRect){
                    update(oldhighlightedRect);
                    update(thehighlightedRect);
                }
                theInsertRow = row(itemAt(pos - QPoint(0, offset()))) + 1;
            }
        }else{
            timeCount++;
        }
    }else if(posYToParent <= scrollMargin && scrollBarValue > 0){
        if(timeCount >= timeLimit){
            emit Interval((posYToParent + 5) / 3);
            emit exceptedValue(scrollBarValue - step);
            if(pos.y() >= offset()){
                thehighlightedRow = row(itemAt(pos - QPoint(0, offset())));

                oldhighlightedRect = thehighlightedRect;
                thehighlightedRect = targetRect(pos);

                if(oldhighlightedRect != thehighlightedRect){
                    update(oldhighlightedRect);
                    update(thehighlightedRect);
                }
                theInsertRow = row(itemAt(pos - QPoint(0, offset()))) + 1;
            }else if(pos.y() >= 0){
                thehighlightedRow = -1;
                update(QRect(0, 0, width(), 80));
                theInsertRow = 0;
            }else{
                thehighlightedRow = -2;
                update(QRect(0, 0, width(), 40));
                theInsertRow = -1;
            }
        }else{
            timeCount++;
        }
    }else{
        timeCount = 0;
    }
}

//theDragRow will be set as -2 when hoverEnter
//triggerd when drag is destroyed
void MyListWidget::updateItemStyle()
{
    emit endDrag();
    //QDrag *drag = qobject_cast<QDrag *>(sender());  //drag might have been destroyed
    //qDebug()<<rect()<<mapFromGlobal(QCursor::pos()); //rect() is so wired that...
    //if(IsDropOutside){
        update(QRect(0, 40 * (theDragRow - 1), width(), 40));  //auto
        update(QRect(0, 40 * theDragRow, width(), 40));
        update(QRect(0, 40 * (theDragRow + 1), width(), 40));  //auto
    //}
    //qDebug()<<"drag is destoryed!" << theDragRow;
}

bool MyListWidget::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::HoverMove:
        if(IsWheeling){
            oldWheelRow = theWheelRow;
            theWheelRow = -1;
            update(0, 40 * oldWheelRow, width(), 40);
            IsWheeling = false;
        }
        break;
    case QEvent::HoverLeave:
        break;
    case QEvent::HoverEnter:
        theDragRow = -2;
        break;
//QEvent::Wheel 当鼠标超过QListWidget范围时就无法检测到
    case QEvent::Wheel:
        if(!IsWheeling) IsWheeling = true;
        break;
//QEvent::MouseButtonRelease检测不到release
    case QEvent::MouseButtonRelease:
        break;
//[0]
    case QEvent::DragResponse:
        qDebug()<<"1111";
        break;
    case QEvent::Drop:
        qDebug()<<"2222";
        break;
    case QEvent::DragMove:
        qDebug()<<"3333";
        break;
    case QEvent::DragEnter:
        qDebug()<<"4444";
        break;
    case QEvent::DragLeave:
        qDebug()<<"5555";
        break;
//[0]
    default:
        break;
    }
    return QListWidget::event(event);
}

const QRect MyListWidget::targetRect(const QPoint &position) const
{
    if(position.y() >= offset())
        return QRect(0, (position.y() - offset()) / 40 * 40, width(), 2 * 40);
    else
        return QRect(0, 0, width(), 40);
}

const QRect MyListWidget::selectedRect(const QPoint &position) const
{
    return QRect(0, position.y() / 40 * 40, width(), 40);
}

//*********************MyItemDelegate***********************MyItemDelegate**************************MyItemDelegate*******//

MyItemDelegate::MyItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void MyItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    MyListWidget *dragWidget = qobject_cast<MyListWidget *>(option.styleObject);
    bool isDraging = dragWidget->isDraging();
    bool isWheeling = dragWidget->isWheeling();
    int theRowByMenu = dragWidget->rowByMenu();

    QRect rect = option.rect;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    QPixmap Image;
    QPixmap Image_hover;
    Image.load(":/listBar_Icon/1.png");
    Image_hover.load(":/listBar_Icon/1_hover.png");

    QColor NormalColor = QColor(245, 245, 247);
    QColor SelectColor_right = QColor(180, 0, 0);
    QColor SelectColor_left = QColor(230, 231, 234);

    int IconSize = 21;
    int Y = (40 - IconSize) / 2 + 2;
    int X = 21;

//    if(option.state & (QStyle::State_MouseOver | QStyle::State_Selected)){
//        if(option.state & QStyle::State_MouseOver){
//            //painter->setBrush(hoverColor);
//            //painter->drawRect(rect);
//            painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image_hover);
//        }
//        if(option.state & QStyle::State_Selected){
//            painter->setBrush(SelectColor_right);
//            painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), 4, rect.height());

//            painter->setBrush(SelectColor_left);
//            painter->drawRect(rect.topLeft().x() + 4, rect.topLeft().y(), rect.width() - 4, rect.height());

//            painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image_hover);
//        }
//    }else{
//        painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image);
//    }

//按照优先级, QStyle::State_Selected排在最前面
    if(option.state & QStyle::State_Selected){
        painter->setBrush(SelectColor_right);
        painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), 4, rect.height());

        painter->setBrush(SelectColor_left);
        painter->drawRect(rect.topLeft().x() + 4, rect.topLeft().y(), rect.width() - 4, rect.height());

        painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image_hover);
    }else if(option.state & QStyle::State_MouseOver){
        painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image_hover);
    }else{
        painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image);
    }

//begin wheel
    if(isWheeling){
        int theWheelRow = dragWidget->wheelRow();

        QStyleOptionViewItem opt(option);

//        if(option.state & QStyle::State_Selected){  //保证item在selected状态下不会被hover
//            opt.state &= ~QStyle::State_MouseOver;
//        }

        if(option.state & QStyle::State_MouseOver){  //处理QScrollBar的wheel
            opt.state &= ~QStyle::State_MouseOver;
        }

        if(index.row() == theWheelRow){
            opt.state |= QStyle::State_MouseOver;
            painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image_hover);
        }else if(option.state & QStyle::State_Selected){
            painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image_hover);
        }else{
            painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image);
        }

        QStyledItemDelegate::paint(painter, opt, index);
        return;
    }
//end wheel

//begin drag
    if(isDraging){
        int theDragRow = dragWidget->dragRow();
        int theSelectedRow = dragWidget->selectedRow();
        int UpRow = dragWidget->highlightedRow();
        int DownRow = UpRow + 1;
        int rowCount = dragWidget->count() - 1;

//处理当拖拽发生时event->pos() != startPos时的bug
//        if(option.state & QStyle::State_MouseOver && index.row() != theDragRow){
//            QStyleOptionViewItem opt(option);
//            opt.state &= ~QStyle::State_MouseOver;
//            QStyledItemDelegate::paint(painter, opt, index);
//        }

//只绘制空隙,而不绘制DropIndicator
        if(index.row() == theSelectedRow && theDragRow != theSelectedRow){
            if(index.row() == UpRow && index.row() != theDragRow - 1){
                int offset = 3;
                QPolygon trianglePolygon_bottomLeft;
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (offset + WIDTH) + 1);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (offset + WIDTH + POLYGON) + 1);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x() + POLYGON, rect.bottomLeft().y() - (offset + WIDTH) + 1);

                QPolygon trianglePolygon_bottomRight;
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x(), rect.bottomRight().y() - (offset + WIDTH) + 1);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x(), rect.bottomRight().y() - (offset + WIDTH + POLYGON) + 1);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() - POLYGON, rect.bottomRight().y() - (offset + WIDTH) + 1);

                painter->setBrush(NormalColor);
                painter->drawPolygon(trianglePolygon_bottomLeft);
                painter->drawPolygon(trianglePolygon_bottomRight);
                painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - (offset + WIDTH) + 1, rect.width(), offset + WIDTH);
                painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - (offset + WIDTH) + 1, rect.width(), offset + WIDTH);
            }else if(index.row() == DownRow && index.row() != theDragRow + 1){
                int offset = 3;

                QPolygon trianglePolygon_topLeft;
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + offset + WIDTH);
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + offset + WIDTH + POLYGON);
                trianglePolygon_topLeft << QPoint(rect.topLeft().x() + POLYGON, rect.topLeft().y() + offset + WIDTH);

                QPolygon trianglePolygon_topRight;
                trianglePolygon_topRight << QPoint(rect.topRight().x(), rect.topRight().y() + offset + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x(), rect.topRight().y() + offset + WIDTH + POLYGON);
                trianglePolygon_topRight << QPoint(rect.topRight().x() - POLYGON, rect.topRight().y() + offset + WIDTH);

                painter->setBrush(NormalColor);
                painter->drawPolygon(trianglePolygon_topLeft);
                painter->drawPolygon(trianglePolygon_topRight);
                painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), rect.width(), offset + WIDTH);
            }
        }

//绘制DropIndicator
        if(index.row() == UpRow && index.row() != theDragRow - 1 && index.row() != theDragRow){
            painter->setBrush(QColor(66, 133, 244));

            if(UpRow == rowCount){
                //到达尾部,三角形向上移动一个WIDTH的距离,以使分隔符宽度*2
                QPolygon trianglePolygon_bottomLeft;
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (POLYGON + WIDTH) + 1 - WIDTH);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1 - WIDTH);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x() + POLYGON, rect.bottomLeft().y() - WIDTH + 1 - WIDTH);

                QPolygon trianglePolygon_bottomRight;
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x(), rect.bottomRight().y() - (POLYGON + WIDTH) + 1 - WIDTH);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x(), rect.bottomRight().y() - WIDTH + 1 - WIDTH);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() - POLYGON, rect.bottomRight().y() - WIDTH + 1 - WIDTH);

                painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - 2 * WIDTH + 1, rect.width(), 2 * WIDTH);  //rect
                painter->drawPolygon(trianglePolygon_bottomLeft);
                painter->drawPolygon(trianglePolygon_bottomRight);
            }
            else {
                //正常情况,组成上半部分(+1是根据实际情况修正)
                QPolygon trianglePolygon_bottomLeft;
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (POLYGON + WIDTH) + 1);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x() + POLYGON, rect.bottomLeft().y() - WIDTH + 1);

                QPolygon trianglePolygon_bottomRight;
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x(), rect.bottomRight().y() - (POLYGON + WIDTH) + 1);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x(), rect.bottomRight().y() - WIDTH + 1);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() - POLYGON, rect.bottomRight().y() - WIDTH + 1);

                painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1, rect.width(), WIDTH);  //rect
                painter->drawPolygon(trianglePolygon_bottomLeft);
                painter->drawPolygon(trianglePolygon_bottomRight);
            }
        }
        else if(index.row() == DownRow && index.row() != theDragRow + 1 && index.row() != theDragRow){
            painter->setBrush(QColor(66, 133, 244));

            if(DownRow == 0){
                //reach the head
                QPolygon trianglePolygon_topLeft;
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + (POLYGON + WIDTH) + WIDTH);
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + WIDTH + WIDTH);
                trianglePolygon_topLeft << QPoint(rect.topLeft().x() + POLYGON, rect.topLeft().y() + WIDTH + WIDTH);

                QPolygon trianglePolygon_topRight;
                trianglePolygon_topRight << QPoint(rect.topRight().x(), rect.topRight().y() + (POLYGON + WIDTH) + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x(), rect.topRight().y() + WIDTH + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x() - POLYGON, rect.topRight().y() + WIDTH + WIDTH);

                painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), rect.width(), 2 * WIDTH);  //rect
                painter->drawPolygon(trianglePolygon_topLeft);
                painter->drawPolygon(trianglePolygon_topRight);
            }
            else{
                //normal
                QPolygon trianglePolygon_topLeft;
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + (POLYGON + WIDTH));
                trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + WIDTH);
                trianglePolygon_topLeft << QPoint(rect.topLeft().x() + POLYGON, rect.topLeft().y() + WIDTH);

                QPolygon trianglePolygon_topRight;
                trianglePolygon_topRight << QPoint(rect.topRight().x(), rect.topRight().y() + (POLYGON + WIDTH));
                trianglePolygon_topRight << QPoint(rect.topRight().x(), rect.topRight().y() + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x() - POLYGON, rect.topRight().y() + WIDTH);

                painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), rect.width(), WIDTH);  //rect
                painter->drawPolygon(trianglePolygon_topLeft);
                painter->drawPolygon(trianglePolygon_topRight);
            }
        }

//update to highlighted the theDragRow
        if(index.row() == theDragRow && theDragRow != theSelectedRow){

            painter->setBrush(QColor(0, 0, 180));
            painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), 4, rect.height());

            painter->setBrush(SelectColor_left);
            painter->drawRect(rect.topLeft().x() + 4, rect.topLeft().y(), rect.width() - 4, rect.height());

            painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image_hover);
//opt.state is used to hightlight the font
            QStyleOptionViewItem opt(option);
            opt.state |= QStyle::State_MouseOver;
            QStyledItemDelegate::paint(painter, opt, index);
            return;
        }

        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
//end drag

//solve the problem that the font still remains highlighted even after drag release outside the viewport
    if(option.state & QStyle::State_MouseOver  && dragWidget->isDropOutside() && index.row() != dragWidget->selectedRow()
    && (index.row() == dragWidget->dragRow() || index.row() == dragWidget->dragRow() + 1 || index.row() == dragWidget->dragRow() - 1)){
        QStyleOptionViewItem opt(option);
        opt.state &= ~QStyle::State_MouseOver;
        QStyledItemDelegate::paint(painter, opt, index);
        painter->setBrush(NormalColor);
        painter->drawRect(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize);
        painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image);
        return;
    }

//update the row besides the menu
    if(index.row() == theRowByMenu){
        painter->setBrush(QColor(0, 180, 0));
        painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), 4, rect.height());

        painter->setBrush(SelectColor_left);
        painter->drawRect(rect.topLeft().x() + 4, rect.topLeft().y(), rect.width() - 4, rect.height());

        painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image_hover);

        QStyleOptionViewItem opt(option);
        opt.state |= QStyle::State_MouseOver;
        QStyledItemDelegate::paint(painter, opt, index);
        return;
    }

    QStyledItemDelegate::paint(painter, option, index);
}

//************************MyScrollBar***********************MyScrollBar**************************MyScrollBar************************//

MyScrollBar::MyScrollBar(QWidget *parent) :
    QScrollBar(parent)
{
}

//void MyScrollBar::mousePressEvent(QMouseEvent *event)
//{
//    QScrollBar::mousePressEvent(event);
//    double pos = static_cast<double>(event->pos().y()) / static_cast<double>(height()); //6和15是修正系数
////防止双击拖动handle时激发此信号
//    if(!isSliderDown()){
//        setValue(static_cast<int>(pos * (maximum() - minimum()) + minimum()));
//    }
//}

void MyScrollBar::wheelEvent(QWheelEvent *event)
{
    QScrollBar::wheelEvent(event);
    emit WheelMsg(event);
}

void MyScrollBar::scrollToValue(int /*value*/)
{
}

bool MyScrollBar::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Show:
        break;
    case QEvent::Hide:
        break;
//ensure the size of handle to be suitable
    case QEvent::Resize:
        setPageStep(height());
        break;
    case QEvent::HoverEnter:
        setStyleSheet(
                    "QScrollBar{background:transparent; width:10px; margin:0px 0px 0px 0px;}"
                    "QScrollBar::handle{background:lightgray; border:0px; border-radius:5px; margin:0px 0px 0px 0px;}"
                    "QScrollBar::handle:hover{background:lightgray;}"
                    "QScrollBar::handle:pressed{background:rgba(200, 200, 200, 255);}"
                    "QScrollBar::sub-page{background:transparent;}"
                    "QScrollBar::add-page{background:transparent;}"
                    "QScrollBar::up-arrow{background:transparent;}"
                    "QScrollBar::down-arrow{background:transparent;}"
                    "QScrollBar::sub-line{background:transparent; height:0px;}"
                    "QScrollBar::add-line{background:transparent; height:0px;}"
                    );
        break;
    case QEvent::HoverLeave:
        setStyleSheet(
                    "QScrollBar{background:transparent; width:10px; margin:0px 0px 0px 0px;}"
                    "QScrollBar::handle{background:rgba(223, 223, 225, 200); border:0px; border-radius:5px; margin:0px 0px 0px 0px;}"
                    "QScrollBar::handle:hover{background:lightgray;}"
                    "QScrollBar::handle:pressed{background:rgba(200, 200, 200, 255);}"
                    "QScrollBar::sub-page{background:transparent;}"
                    "QScrollBar::add-page{background:transparent;}"
                    "QScrollBar::up-arrow{background:transparent;}"
                    "QScrollBar::down-arrow{background:transparent;}"
                    "QScrollBar::sub-line{background:transparent; height:0px;}"
                    "QScrollBar::add-line{background:transparent; height:0px;}"
                    );
        break;
    default:
        break;
    }
    return QScrollBar::event(event);
}

void MyScrollBar::getSliderRange(int min, int max)
{
    setMinimum(min);
    setMaximum(max);
    if(max == min) hide();
    else show();
}

//************************HiddenScrollBar*********************HiddenScrollBar************************HiddenScrollBar************************//

HiddenScrollBar::HiddenScrollBar(QWidget *parent) :
    QScrollBar(parent)
{
}

void HiddenScrollBar::wheelEvent(QWheelEvent *event)
{
    QScrollBar::wheelEvent(event);
    emit Wheel();
}

void HiddenScrollBar::wheelHelper(QWheelEvent *event)
{
    HiddenScrollBar::wheelEvent(event);
}

//**************************MyMenu**************************MyMenu**********************************MyMenu*********************************//

MyMenu::MyMenu(QWidget *parent) :
    QMenu(parent)
{
}

MyMenu::MyMenu(const QString &title) :
    QMenu(title)
{
}

bool MyMenu::event(QEvent *event)
{
    static bool class_amended = false;
    if (event->type() == QEvent::WinIdChange){
        HWND hwnd = reinterpret_cast<HWND>(winId());
        if (class_amended == false){
            class_amended = true;
            DWORD class_style = ::GetClassLong(hwnd, GCL_STYLE);
            class_style &= ~static_cast<unsigned long>(CS_DROPSHADOW);
            ::SetClassLong(hwnd, GCL_STYLE, static_cast<long>(class_style));
        }

        DWMNCRENDERINGPOLICY val = DWMNCRP_ENABLED;
        ::DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &val, sizeof(DWMNCRENDERINGPOLICY));
        // This will turn OFF the shadow
        // MARGINS m = {0};
        // This will turn ON the shadow
        MARGINS m = {-1, -1, -1, -1};
        HRESULT hr = ::DwmExtendFrameIntoClientArea(hwnd, &m);
        if( SUCCEEDED(hr) ){
            //do more things
        }
    }
    return QWidget::event(event);
}

void MyMenu::closeEvent(QCloseEvent *event)
{
    emit MenuIsClosed();
    event->accept();
}

//**************************QActionWidget************************QActionWidget**************************QActionWidget************************//

QActionWidget::QActionWidget(QWidget *parent) :
    QWidget(parent)
{
    initUi();
}

void QActionWidget::initUi()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setFixedSize(240, 40);

    ImageButton = new QPushButton(this);
    ImageButton->setFixedSize(30, 30);
    ImageButton->setIconSize(QSize(28, 28));
    ImageButton->setFlat(true);
    ImageButton->setFocusPolicy(Qt::NoFocus);
    ImageButton->setAttribute(Qt::WA_TransparentForMouseEvents,true);  //鼠标穿透效果

    TextLabel = new QLabel(this);
    TextLabel->setFixedSize(150, 26);
    TextLabel->setFocusPolicy(Qt::NoFocus);
    TextLabel->setFont(QFont("Microsoft YaHei", 10, QFont::Normal));
    TextLabel->setStyleSheet("QLabel{color:rgba(0, 0, 0, 255);}");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->addSpacing(10);
    layout->addWidget(ImageButton);
    layout->addSpacing(10);
    layout->addWidget(TextLabel);
    layout->addStretch();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

void QActionWidget::setupWidget(const QIcon &icon, const QString &str)
{
    ImageButton->setIcon(icon);
    TextLabel->setText(str);
    Img = icon;
}

void QActionWidget::setIconSize(int size)
{
    ImageButton->setIconSize(QSize(size, size));
}

bool QActionWidget::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Enter:
        IsHover = true;
        update();
        break;
    case QEvent::Leave:
        IsHover = false;
        update();
        break;
    default:
        break;
    }
    return QWidget::event(event);
}

void QActionWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);

    if(!IsHover){
        painter.setBrush(QColor(255, 255, 255, 255));
        painter.drawRect(rect());
    }else{
        painter.setBrush(QColor(230, 230, 230, 255));
        painter.drawRect(rect());
    }
}

//*************************PPushButton**************************PPushButton*****************************PPushButton***********************//

PPushButton::PPushButton(QWidget *parent) :
    QPushButton(parent),
    btnState(State_Normal),
    radius(0)
{
    setMouseTracking(true);

    //setCursor(Qt::PointingHandCursor);

    //! setMask controls the rect of button very well !
    setMask(QRegion(3, 3, 34, 34, QRegion::Ellipse));
    //ellipse.addEllipse(6, 6, 28, 28);
    ellipse.addEllipse(QPoint(20, 20), 16, 16);


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=](){ radius++; update(); });

    //! &QTimer::start() vs &QTimer::start(int msec)
    //connect(this, &PPushButton::pressed, timer, static_cast<void(QTimer::*)()>(&QTimer::start));
    //connect(this, &PPushButton::clicked, timer, [=](){qDebug()<< "clicked" << QDateTime::currentDateTime();});
    //connect(this, &PPushButton::released, timer, [=](){qDebug()<< "released" << QDateTime::currentDateTime();});
}


//pressd() ---215ms--- released() ---0ms--- clicked()
void PPushButton::mousePressEvent(QMouseEvent *event)
{   
    if(ellipse.contains(event->pos())){
        QPushButton::mousePressEvent(event);
        clickPos = event->pos();
        QPoint center = QPoint(width() / 2, width() / 2);
        int interval = static_cast<int>((215.0f / ((clickPos - center).manhattanLength() + 16.0f)) + 0.5f);
        timer->start(interval);
    }
}

void PPushButton::mouseReleaseEvent(QMouseEvent *event)
{
    QPushButton::mouseReleaseEvent(event);

    timer->stop();

    if(ellipse.contains(event->pos())) { radius = 40; emit pButtonClicked(); }
    else radius = 0;
    update();
}

void PPushButton::mouseMoveEvent(QMouseEvent *event)
{
    QPushButton::mouseMoveEvent(event);

    state oldState = btnState;
    if(ellipse.contains(event->pos())){
        btnState = State_Hover;
        setCursor(Qt::PointingHandCursor);
    }else{
        btnState = State_Normal;
        unsetCursor();
    }
    if(oldState != btnState) update();
}

void PPushButton::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);
    btnState = State_Normal;
    unsetCursor();
    update();
}

void PPushButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);

    if(btnState == State_Hover){
        painter.setBrush(QColor(235, 235, 235, 255));
        painter.drawPath(ellipse);
    }

    if(radius > 0){
        painter.setBrush(QColor(225, 225, 225));
        QPainterPath circle;
        circle.addEllipse(clickPos, radius, radius);
        painter.drawPath(circle.intersected(ellipse));
    }

    QPushButton::paintEvent(event);
}

//************************ListBar类******************************ListBar类********************************ListBar类************************//

ListBar::ListBar(QWidget *parent) :
    QWidget(parent)
{
    initUi();
}

void ListBar::initUi()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    setFixedWidth(250);
    resize(250, 750 - 120 - 70);

    RecommendLabel = new QLabel(this);
    RecommendLabel->setFixedSize(100, 40);
    RecommendLabel->setText("推荐");
    RecommendLabel->setFont(QFont("Microsoft YaHei", 9, QFont::Normal));
    RecommendLabel->setStyleSheet("QLabel{color:rgba(244, 66, 150, 255); padding-left:7px; border:0px solid rgb(0, 0, 0)}");
    RecommendLabel->setFocusPolicy(Qt::NoFocus);

    MyMusicLabel = new QLabel(this);
    MyMusicLabel->setFixedSize(100, 40);
    MyMusicLabel->setText("我的音乐");
    MyMusicLabel->setFont(QFont("Microsoft YaHei", 9, QFont::Normal));
    MyMusicLabel->setStyleSheet("QLabel{color:rgba(244, 66, 150, 255); padding-left:7px; border:0px solid rgb(0, 0, 0)}");
    MyMusicLabel->setFocusPolicy(Qt::NoFocus);

    CreatedDirButton  = new QPushButton(this);
    CreatedDirButton->setFixedSize(250, 40);
    CreatedDirButton->setFlat(true);
    CreatedDirButton->setIcon(QIcon(":/listBar_Icon/right_arrow.png"));
    CreatedDirButton->setIconSize(QSize(21, 21));
    CreatedDirButton->setText("创建的歌单");
    CreatedDirButton->setFont(QFont("Microsoft YaHei", 9, QFont::Normal));
    CreatedDirButton->setStyleSheet("QPushButton{color:rgba(244, 66, 150, 255); text-align:left center; border:0px solid rgb(0, 0, 0);}");
    CreatedDirButton->setCursor(Qt::PointingHandCursor);
    CreatedDirButton->setFocusPolicy(Qt::NoFocus);
    CreatedDirButton->installEventFilter(this);
    connect(CreatedDirButton, &QPushButton::clicked, this, &ListBar::on_CreatedDirButton_Clicked);

    AddDirButton = new PPushButton(this);
    AddDirButton->setFixedSize(40, 40);
    AddDirButton->setFlat(true);
    AddDirButton->setIcon(QIcon(":/listBar_Icon/add.png"));
    AddDirButton->setIconSize(QSize(18, 18));
    AddDirButton->setStyleSheet("QPushButton{border:0px solid rgb(0, 0, 0);}");
    //AddDirButton->setCursor(Qt::PointingHandCursor);
    AddDirButton->setFocusPolicy(Qt::NoFocus);
    //AddDirButton->installEventFilter(this);
    connect(AddDirButton, &PPushButton::pButtonClicked, this, &ListBar::on_AddDirButton_Clicked);

    QHBoxLayout *childLayout = new QHBoxLayout();
    childLayout->setSpacing(0);
    childLayout->addWidget(CreatedDirButton);
    childLayout->addSpacing(-70);  //(200, 430)
    childLayout->addWidget(AddDirButton);
    childLayout->setContentsMargins(0, 0, 0, 0);

    RecommendList = new CustomListWidget(this);
    //RecommendList->setIconSize(QSize(25, 25));
    RecommendList->setCursor(Qt::PointingHandCursor);
    RecommendList->setFocusPolicy(Qt::NoFocus);  //这样可禁用tab键和上下方向键并且除去复选框
    RecommendList->setFixedHeight(160);
    //RecommendList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    RecommendList->setStyleSheet(
                //"*{outline:0px;}"  //除去复选框
                "QListWidget{background:rgb(245, 245, 247); border:0px solid rgb(0, 0, 0); margin-top:0px; margin-bottom:0px;}"
                "QListWidget::Item{height:40px; border:0px; padding-left:17px;}"
                //"QListWidget::Item:hover{color:rgba(0, 0, 0, 255);}"
                "QListWidget::Item:selected{background:rgb(230, 231, 234); color:rgb(0, 0, 0); border-left:4px solid rgb(180, 0, 0);}"
                "QListWidget::Item:selected:active{background:rgb(230, 231, 234); color:rgb(0, 0, 0); border-left:4px solid rgb(180, 0, 0);}");
    RecommendDelegate = new CustomItemDelegate();
    RecommendList->setItemDelegate(RecommendDelegate);
    connect(RecommendList,
            &CustomListWidget::itemSelectionChanged,  //相比于itemClicked更快响应
            this,
            &ListBar::on_ListWidget_Clicked);

    Item_1 = new QListWidgetItem(RecommendList);
    //Item_1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
    ItemWidget_1 = new ItemContentsWidget(RecommendList);
    ItemWidget_1->setupWidget(QIcon(":/listBar_Icon/1.png"), QIcon(":/listBar_Icon/1_hover.png"), "发现音乐");
    ItemWidget_1->installEventFilter(this);
    connect(this, &ListBar::isItem_1Selected, ItemWidget_1, &ItemContentsWidget::isItemSelected);
    RecommendList->setItemWidget(Item_1, ItemWidget_1);

    Item_2 = new QListWidgetItem(RecommendList);
    ItemWidget_2 = new ItemContentsWidget(RecommendList);
    ItemWidget_2->setupWidget(QIcon(":/listBar_Icon/2.png"), QIcon(":/listBar_Icon/2_hover.png"), "私人FM");
    ItemWidget_2->installEventFilter(this);
    connect(this, &ListBar::isItem_2Selected, ItemWidget_2, &ItemContentsWidget::isItemSelected);
    RecommendList->setItemWidget(Item_2, ItemWidget_2);

    Item_3 = new QListWidgetItem(RecommendList);
    ItemWidget_3 = new ItemContentsWidget(RecommendList);
    ItemWidget_3->setupWidget(QIcon(":/listBar_Icon/3.png"), QIcon(":/listBar_Icon/3_hover.png"), "MV");
    ItemWidget_3->installEventFilter(this);
    connect(this, &ListBar::isItem_3Selected, ItemWidget_3, &ItemContentsWidget::isItemSelected);
    RecommendList->setItemWidget(Item_3, ItemWidget_3);

    Item_4 = new QListWidgetItem(RecommendList);
    ItemWidget_4 = new ItemContentsWidget(RecommendList);
    ItemWidget_4->setupWidget(QIcon(":/listBar_Icon/4.png"), QIcon(":/listBar_Icon/4_hover.png"), "朋友");
    ItemWidget_4->installEventFilter(this);
    connect(this, &ListBar::isItem_4Selected, ItemWidget_4, &ItemContentsWidget::isItemSelected);
    RecommendList->setItemWidget(Item_4, ItemWidget_4);

    MyMusicList = new CustomListWidget(this);
    //MyMusicList->setIconSize(QSize(25, 25));
    MyMusicList->setCursor(Qt::PointingHandCursor);
    MyMusicList->setFocusPolicy(Qt::NoFocus);  //这样可禁用tab键和上下方向键并且除去复选框
    MyMusicList->setFixedHeight(160);
    MyMusicList->setStyleSheet(
                //"*{outline:0px;}"  //除去复选框
                "QListWidget{background:rgb(245, 245, 247); border:0px; margin-top:0px; margin-bottom:0px;}"
                "QListWidget::Item{height:40px; border:0px; padding-left:17px;}"
                "QListWidget::Item:hover{color:rgba(0, 0, 0, 255);}"
                "QListWidget::Item:selected{background:rgb(230, 231, 234); color:rgb(0, 0, 0); border-left:4px solid rgb(180, 0, 0);}"
                "QListWidget::Item:selected:active{background:rgb(230, 231, 234); color:rgb(0, 0, 0); border-left:4px solid rgb(180, 0, 0);}");

    MyMusicDelegate = new CustomItemDelegate();
    MyMusicList->setItemDelegate(MyMusicDelegate);
    connect(MyMusicList,
            &CustomListWidget::itemSelectionChanged,  //相比于itemClicked更快响应
            this,
            &ListBar::on_ListWidget_Clicked);

    Item_5 = new QListWidgetItem(MyMusicList);
    ItemWidget_5 = new ItemContentsWidget(MyMusicList);
    ItemWidget_5->setupWidget(QIcon(":/listBar_Icon/5.png"), QIcon(":/listBar_Icon/5_hover.png"), "本地音乐");
    ItemWidget_5->installEventFilter(this);
    connect(this, &ListBar::isItem_5Selected, ItemWidget_5, &ItemContentsWidget::isItemSelected);
    MyMusicList->setItemWidget(Item_5, ItemWidget_5);

    Item_6 = new QListWidgetItem(MyMusicList);
    ItemWidget_6 = new ItemContentsWidget(MyMusicList);
    ItemWidget_6->setupWidget(QIcon(":/listBar_Icon/6.png"), QIcon(":/listBar_Icon/6_hover.png"), "下载管理");
    ItemWidget_6->installEventFilter(this);
    connect(this, &ListBar::isItem_6Selected, ItemWidget_6, &ItemContentsWidget::isItemSelected);
    MyMusicList->setItemWidget(Item_6, ItemWidget_6);

    Item_7 = new QListWidgetItem(MyMusicList);
    ItemWidget_7 = new ItemContentsWidget(MyMusicList);
    ItemWidget_7->setupWidget(QIcon(":/listBar_Icon/7.png"), QIcon(":/listBar_Icon/7_hover.png"), "我的音乐云盘");
    ItemWidget_7->installEventFilter(this);
    connect(this, &ListBar::isItem_7Selected, ItemWidget_7, &ItemContentsWidget::isItemSelected);
    MyMusicList->setItemWidget(Item_7, ItemWidget_7);

    Item_8 = new QListWidgetItem(MyMusicList);
    ItemWidget_8 = new ItemContentsWidget(MyMusicList);
    ItemWidget_8->setupWidget(QIcon(":/listBar_Icon/8.png"), QIcon(":/listBar_Icon/8_hover.png"), "我的收藏");
    ItemWidget_8->installEventFilter(this);
    connect(this, &ListBar::isItem_8Selected, ItemWidget_8, &ItemContentsWidget::isItemSelected);
    MyMusicList->setItemWidget(Item_8, ItemWidget_8);

    CreatedDirList = new MyListWidget(this);
    //CreatedDirList->setIconSize(QSize(25, 25));
    CreatedDirList->setFont(QFont("Microsoft YaHei", 9, QFont::Normal));
    //CreatedDirList->setCursor(Qt::PointingHandCursor);
    CreatedDirList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    CreatedDirList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    CreatedDirList->setTextElideMode(Qt::ElideRight);  //字符超长检测
    CreatedDirList->setFocusPolicy(Qt::NoFocus);  //这样可禁用tab键和上下方向键并且除去复选框
    //CreatedDirList->setFixedHeight(160);
    CreatedDirList->setStyleSheet(
              //"*{outline:0px;}"  //除去复选框
                "QListWidget{color:rgba(245, 66, 66, 200); background:rgb(245, 245, 247); border:0px;}"
                "QListWidget::Item{color:rgba(245, 66, 66, 200); height:40px; border:0px; padding-left:46px;}"
                "QListWidget::Item:hover{color:rgba(66, 133, 245, 255);}"
                "QListWidget::Item:selected{color:rgba(66, 133, 245, 255);}"
                "QListWidget::Item:selected:active{color:rgba(66, 133, 245, 255);}"
                );
    CreatedDirList->hide();

    CreatedDirDelegate = new MyItemDelegate();
    CreatedDirList->setItemDelegate(CreatedDirDelegate);
    connect(CreatedDirList,
            &MyListWidget::itemPressed,  //相比于itemClicked更快响应
            this,
            &ListBar::on_CreatedDirList_Clicked);
    connect(CreatedDirList, &MyListWidget::ContextMenuClicked, this, &ListBar::showContextMenu);
    connect(this, &ListBar::limitedHeight, CreatedDirList, &MyListWidget::getLimitedHeight);

    QVBoxLayout *Layout = new QVBoxLayout();
    Layout->setSpacing(0);
    Layout->addWidget(RecommendLabel);
    Layout->addWidget(RecommendList);
    Layout->addSpacing(15);
    Layout->addWidget(MyMusicLabel);
    Layout->addWidget(MyMusicList);
    Layout->addSpacing(15);
    Layout->addLayout(childLayout);
    Layout->addWidget(CreatedDirList);
    Layout->addStretch();
    Layout->setContentsMargins(0, 0, 0, 0);  //上下左右边距

    mainWidget = new QWidget(this);
    mainWidget->setLayout(Layout);
    mainWidget->setFixedSize(250, 607);

    theShownScrollBar = new MyScrollBar(mainWidget);
    theShownScrollBar->setStyleSheet(
                "QScrollBar{background:transparent; width:10px; margin:0px 0px 0px 0px;}"
                "QScrollBar::handle{background:rgba(223, 223, 225, 200); border:0px; border-radius:5px; margin:0px 0px 0px 0px;}"
                "QScrollBar::handle:hover{background:lightgray;}"
                "QScrollBar::handle:pressed{background:rgba(200, 200, 200, 255);}"
                "QScrollBar::sub-page{background:transparent;}"
                "QScrollBar::add-page{background:transparent;}"
                "QScrollBar::up-arrow{background:transparent;}"
                "QScrollBar::down-arrow{background:transparent;}"
                "QScrollBar::sub-line{background:transparent; height:0px;}"
                "QScrollBar::add-line{background:transparent; height:0px;}"
                );
    connect(theShownScrollBar, &MyScrollBar::valueChanged, CreatedDirList, &MyListWidget::getScrollBarValue);
    connect(theShownScrollBar, &MyScrollBar::rangeChanged, CreatedDirList, &MyListWidget::getscrollBarRange);
    connect(CreatedDirList, &MyListWidget::exceptedValue, theShownScrollBar, &MyScrollBar::setValue);
    //connect(theShownScrollBar, &MyScrollBar::Wheel, CreatedDirList, &MyListWidget::HandleScrollBarWheeled);

    ScrollArea = new QScrollArea(this);
    ScrollArea->setFocusPolicy(Qt::NoFocus);  //禁止键盘方向键影响
    ScrollArea->setFrameShape(QFrame::NoFrame);
    HiddenScrollBar *theHidedScrollBar = new HiddenScrollBar(ScrollArea);
    ScrollArea->setVerticalScrollBar(theHidedScrollBar);
//horizontalScrollBar() is off(hide) and disabled
    ScrollArea->horizontalScrollBar()->setEnabled(false);
    ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//verticalScrollBar() is off(hide) but enabled
    ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ScrollArea->setWidget(mainWidget);

    connect(theHidedScrollBar, &HiddenScrollBar::Wheel, CreatedDirList, &MyListWidget::HandleScrollBarWheeled);
    connect(theHidedScrollBar, &HiddenScrollBar::rangeChanged, theShownScrollBar, &MyScrollBar::getSliderRange);
    connect(theHidedScrollBar, &HiddenScrollBar::valueChanged, theShownScrollBar, &MyScrollBar::setValue);
    connect(theShownScrollBar, &MyScrollBar::valueChanged, theHidedScrollBar, &HiddenScrollBar::setValue);
    connect(theShownScrollBar, &MyScrollBar::WheelMsg, theHidedScrollBar, &HiddenScrollBar::wheelHelper);

    QHBoxLayout *fatherLayout = new QHBoxLayout(this);
    fatherLayout->setSpacing(0);
    fatherLayout->addWidget(ScrollArea);
    fatherLayout->addSpacing(-10);
    fatherLayout->addWidget(theShownScrollBar);
    fatherLayout->setContentsMargins(0, 0, 0, 0);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(245, 245, 247));
    setAutoFillBackground(true);
    setPalette(pal);

    listFrame = new QFrame(this);  //放在前面就无法显示
    listFrame->setGeometry(0, 0, 250, 200);
    listFrame->setStyleSheet("QFrame{border-right:1px solid rgb(200, 200, 200); border-top:0px; border-bottom:0px; border-left:0px;}");
    listFrame->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    //listFrame->raise();

    RecommendList->setCurrentItem(Item_1);  //初始化时Item_1被选中

    CreatedDirList->addItem("深海鱼的眼泪");
    CreatedDirList->addItem("彼岸花的悲殇");
    CreatedDirList->addItem("向日葵的绚烂");
    CreatedDirList->addItem("维也纳的艺术");
    CreatedDirList->addItem("谁是谁的谁");
    CreatedDirList->addItem("梦里悲几许");
    CreatedDirList->addItem("回忆是道殇");
    CreatedDirList->addItem("繁华沦为泪");
    CreatedDirList->addItem("倾似谁的城");
    CreatedDirList->addItem("你若在心上");
    CreatedDirList->addItem("等你几年又何妨");
    CreatedDirList->addItem("典型的音乐范r");
    CreatedDirList->addItem("专业的艺术范r");
    CreatedDirList->addItem("美腻的国际范r");
    CreatedDirList->addItem("天真的小腐女r");
    CreatedDirList->addItem("职业的卖萌娃r");
    CreatedDirList->addItem("邪恶的两面范r");
    CreatedDirList->addItem("不白旳白天");
    CreatedDirList->addItem("我背着琴去远方");
    CreatedDirList->addItem("暗涌旳黑夜");
    CreatedDirList->addItem("眼泪落在琴弦上");
    CreatedDirList->addItem("画一方禁地");
    CreatedDirList->addItem("用音乐把命延长");
    CreatedDirList->addItem("判你于无期");
    CreatedDirList->addItem("来囚禁我的忧伤");
    CreatedDirList->addItem("又让我想起你");
    CreatedDirList->addItem("the sound of music");
    CreatedDirList->addItem("you and me");
    CreatedDirList->addItem("blue and green");
    CreatedDirList->addItem("玫瑰、花葬");
    CreatedDirList->addItem("星座、书上");
    CreatedDirList->addItem("清明、雨上");
    CreatedDirList->addItem("七号、公园");
    CreatedDirList->addItem("你若、成风");
    CreatedDirList->addItem("四人游");
    CreatedDirList->addItem("因为你");
    CreatedDirList->addItem("自以为");
    CreatedDirList->addItem("够不够");
    CreatedDirList->addItem("小小虫");
    CreatedDirList->addItem("如果爱");
    CreatedDirList->addItem("春风吹");
    CreatedDirList->addItem("没有什么不同");
    CreatedDirList->addItem("一天的回忆");
    CreatedDirList->setFixedHeight(CreatedDirList->count() * 40);

}

void ListBar::on_ListWidget_Clicked()
{
    if(QObject::sender() == RecommendList){

        emit isItem_1Selected(Item_1->isSelected());
        if(Item_1->isSelected()){
            emit Item_1_Clicked();
            MyMusicList->clearSelection();
            CreatedDirList->clearSelection();
        }else{
        }
        emit isItem_2Selected(Item_2->isSelected());
        if(Item_2->isSelected()){
            emit Item_2_Clicked();
            MyMusicList->clearSelection();
            CreatedDirList->clearSelection();
        }else{
        }
        emit isItem_3Selected(Item_3->isSelected());
        if(Item_3->isSelected()){
            emit Item_3_Clicked();
            MyMusicList->clearSelection();
            CreatedDirList->clearSelection();
        }else{
        }
        emit isItem_4Selected(Item_4->isSelected());
        if(Item_4->isSelected()){
            emit Item_4_Clicked();
            MyMusicList->clearSelection();
            CreatedDirList->clearSelection();
        }else{
        }
    }
    else if(QObject::sender() == MyMusicList){
        emit isItem_5Selected(Item_5->isSelected());
        if(Item_5->isSelected()){
            emit Item_5_Clicked();
            RecommendList->clearSelection();
            CreatedDirList->clearSelection();
        }else{
        }
        emit isItem_6Selected(Item_6->isSelected());
        if(Item_6->isSelected()){
            emit Item_6_Clicked();
            RecommendList->clearSelection();
            CreatedDirList->clearSelection();
        }else{
        }
        emit isItem_7Selected(Item_7->isSelected());
        if(Item_7->isSelected()){
            emit Item_7_Clicked();
            RecommendList->clearSelection();
            CreatedDirList->clearSelection();
        }else{
        }
        emit isItem_8Selected(Item_8->isSelected());
        if(Item_8->isSelected()){
            emit Item_8_Clicked();
            RecommendList->clearSelection();
            CreatedDirList->clearSelection();
        }else{
        }
    }
}

void ListBar::on_CreatedDirList_Clicked(QListWidgetItem *item)
{
    RecommendList->clearSelection();
    MyMusicList->clearSelection();
    QString str = item->text();
}

bool ListBar::eventFilter(QObject *obj, QEvent *event)
{
    switch(event->type()){
    case QEvent::Resize:
        listFrame->setFixedHeight(height());
        emit limitedHeight(height());
        break;
    case QEvent::HoverEnter:
        if(obj == AddDirButton){
            AddDirButton->setIcon(QIcon(":/listBar_Icon/add_hover.png"));
        }
        else if(obj == CreatedDirButton){
            if(IsFolded){
                CreatedDirButton->setIcon(QIcon(":/listBar_Icon/right_arrow_hover.png"));
            }else{
                CreatedDirButton->setIcon(QIcon(":/listBar_Icon/down_arrow_hover.png"));
            }
        }
        break;
    case QEvent::HoverLeave:
        if(obj == AddDirButton){
            AddDirButton->setIcon(QIcon(":/listBar_Icon/add.png"));
        }
        else if(obj == CreatedDirButton){
            if(IsFolded){
                CreatedDirButton->setIcon(QIcon(":/listBar_Icon/right_arrow.png"));
            }else{
                CreatedDirButton->setIcon(QIcon(":/listBar_Icon/down_arrow.png"));
            }
        }
        break;
    case QEvent::Enter:  //for widget
        break;
    case QEvent::Leave:  //for widget
        break;
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}

void ListBar::on_AddDirButton_Clicked()
{
    addDirWidget = new AddDirWidget(this);
    connect(addDirWidget, &AddDirWidget::Confirmed, this, &ListBar::insert);
    connect(addDirWidget, &AddDirWidget::highlightButtonIcon, [=](){
        AddDirButton->setIcon(QIcon(QStringLiteral(":/listBar_Icon/add_hover.png")));
    });
    connect(addDirWidget, &AddDirWidget::restoreButtonIcon, [=](){
        AddDirButton->setIcon(QIcon(QStringLiteral(":/listBar_Icon/add.png")));
        AddDirButton->restoreIcon(QCursor::pos());
    });
    addDirWidget->move(mapToGlobal(AddDirButton->pos()
                                   + QPoint(AddDirButton->width() - TRIANGLE_HEIGHT_1 - addDirWidget->X_offset,
                                            AddDirButton->height() / 2 - addDirWidget->height() / 2)
                                   - QPoint(0, theShownScrollBar->value())));  //添加Popup属性后需使用mapToGlobal
    addDirWidget->show();
    addDirWidget->activateWindow();  //解决无法输入中文的问题
}

void ListBar::on_CreatedDirButton_Clicked()
{
    int ordinaryHeight = CreatedDirButton->pos().y() + CreatedDirButton->height();  //470
    if(IsFolded){
        mainWidget->setFixedHeight(ordinaryHeight + 40 * CreatedDirList->count());
        CreatedDirButton->setIcon(QIcon(":/listBar_Icon/down_arrow_hover.png"));
        CreatedDirList->show();  //必须要放在mainWidget后面执行
        IsFolded = false;
    }else{
        CreatedDirList->hide();
        CreatedDirButton->setIcon(QIcon(":/listBar_Icon/right_arrow_hover.png"));
        IsFolded = true;
        mainWidget->setFixedHeight(ordinaryHeight);
    }
}

void ListBar::insert(const QString &str)
{
    QListWidgetItem *newItem = new QListWidgetItem(); //parent can noe be asserted here;
    newItem->setText(str);
    newItem->setTextAlignment(Qt::AlignVCenter);
    CreatedDirList->insertItem(0, newItem);
    CreatedDirList->update();

    int count = CreatedDirList->count();
    CreatedDirList->setFixedHeight(40 * count);
    mainWidget->setFixedHeight(470 + 40 * count);

    CreatedDirList->show();
    CreatedDirButton->setIcon(QIcon(":/listBar_Icon/down_arrow.png"));
    IsFolded = false;
}

void ListBar::showContextMenu(int row)
{
    theRowByMenu = row;
    if(Menu){
        Menu->exec(QCursor::pos());
        return;
    }
    //CCustomStyle *MyStyle = new CCustomStyle();
    Menu = new MyMenu(this);
    //Menu->setStyle(MyStyle);
    Menu->setFixedWidth(250);
    Menu->setFont(QFont("Microsoft YaHei", 10, QFont::Normal));
    Menu->setFocusPolicy(Qt::NoFocus);
    Menu->setStyleSheet("QMenu{background-color:rgba(255, 255, 255, 255); border:0px solid rgb(200, 200, 200); margin:5px;}"
                        "QMenu::item{height:40px; color:rgb(0, 0, 0); background-color:rgba(255, 255, 255, 255);}"//对QWidgetAction无用
                        "QMenu::item:press{background-color:rgba(230, 230, 230, 255);}"//对QWidgetAction无用
                        );
    connect(Menu, &MyMenu::MenuIsClosed, CreatedDirList, &MyListWidget::HandleMenuClosed);

    play = new QWidgetAction(Menu);
    play_ActionWidget = new QActionWidget();
    play_ActionWidget->setupWidget(QIcon(":/listBar_Icon/action_play.png"), "播放(Enter)");
    play->setDefaultWidget(play_ActionWidget);
    connect(play, &QWidgetAction::triggered, this, &ListBar::HandleMenuClicked);
    Menu->addAction(play);

    playnext = new QWidgetAction(Menu);
    playnext_ActionWidget = new QActionWidget();
    playnext_ActionWidget->setupWidget(QIcon(":/listBar_Icon/action_play.png"), "下一首播放");
    playnext->setDefaultWidget(playnext_ActionWidget);
    connect(playnext, &QWidgetAction::triggered, this, &ListBar::HandleMenuClicked);
    Menu->addAction(playnext);

    share = new QWidgetAction(Menu);
    share_ActionWidget = new QActionWidget();
    share_ActionWidget->setupWidget(QIcon(":/listBar_Icon/action_play.png"), "分享");
    share->setDefaultWidget(share_ActionWidget);
    connect(share, &QWidgetAction::triggered, this, &ListBar::HandleMenuClicked);
    Menu->addAction(share);

    rename = new QWidgetAction(Menu);
    rename_ActionWidget = new QActionWidget();
    rename_ActionWidget->setupWidget(QIcon(":/listBar_Icon/action_play.png"), "重命名歌单");
    rename->setDefaultWidget(rename_ActionWidget);
    connect(rename, &QWidgetAction::triggered, this, &ListBar::HandleMenuClicked);
    Menu->addAction(rename);

    remove = new QWidgetAction(Menu);
    remove_ActionWidget = new QActionWidget();
    remove_ActionWidget->setupWidget(QIcon(":/listBar_Icon/action_play.png"), "删除歌单");
    remove->setDefaultWidget(remove_ActionWidget);
    connect(remove, &QWidgetAction::triggered, this, &ListBar::HandleMenuClicked);
    Menu->addAction(remove);

    Menu->exec(QCursor::pos());
}

void ListBar::HandleMenuClicked()
{
    QWidgetAction *triggeredAction = qobject_cast<QWidgetAction *>(sender());
    if(triggeredAction == play){

    }else if(triggeredAction == playnext){

    }else if(triggeredAction == share){

    }else if(triggeredAction == rename){

    }else if(triggeredAction == remove){
        delete CreatedDirList->takeItem(theRowByMenu);
        int count = CreatedDirList->count();
        CreatedDirList->setFixedHeight(40 * count);
        mainWidget->setFixedHeight(470 + 40 * count);
    }
}

