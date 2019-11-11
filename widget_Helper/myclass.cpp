#include "widget_Helper/myclass.h"

MyPushButton::MyPushButton(QWidget *parent) :
    QPushButton(parent)
{
}

void MyPushButton::setUpIcon(const QIcon &icon, const QIcon &icon_hover)
{
    Img = icon;
    Img_hover = icon_hover;
}

bool MyPushButton::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Enter:
        setIcon(Img_hover);
        break;
    case QEvent::Leave:
        setIcon(Img);
        break;
    default:
        break;
    }
    return QPushButton::event(event);
}

TestListWidget::TestListWidget(QWidget *parent) :
    QListWidget(parent)
{
    //setMouseTracking(true);
    //setDragEnabled(true);
    setAcceptDrops(true);
    //setDropIndicatorShown(false);
    //setDefaultDropAction(Qt::MoveAction);

    //setSelectionMode(QAbstractItemView::MultiSelection);

    //connect(this, &TestListWidget::itemSelectionChanged, this, &TestListWidget::updateSelectedIcon);

    MyTimer *scrollBarTimer = new MyTimer();
    //scrollBarTimer->setInterval(10);
    QThread *timerThread = new QThread(this);
    scrollBarTimer->moveToThread(timerThread);
    connect(this, &TestListWidget::beginDrag, scrollBarTimer, &MyTimer::beginTimer);
    connect(this, &TestListWidget::endDrag, scrollBarTimer, &MyTimer::stopTimer);
    connect(this, &TestListWidget::Interval, scrollBarTimer, &MyTimer::getInterval);
    connect(scrollBarTimer, &MyTimer::timeout, this, &TestListWidget::doAutoScroll);
    connect(timerThread, &QThread::finished, scrollBarTimer, &MyTimer::deleteLater);
    timerThread->start();
}

void TestListWidget::updateSelectedIcon()
{
    oldSelectedItem = theSelectedItem;
    theSelectedItem = static_cast<TestListWidgetItem *>(currentItem());

    if(oldSelectedItem != theSelectedItem){
        if(oldSelectedItem) {
            //oldSelectedItem->setTextColor(Color);
            //qDebug()<<oldSelectedItem->text()<<oldSelectedItem->textColor()<<Color_hover;
            oldSelectedItem->setIcon(oldSelectedItem->Img);
        }
        if(theSelectedItem) {
            //theSelectedItem->setTextColor(Color);
            //qDebug()<<theSelectedItem->text()<<theSelectedItem->textColor()<<Color_hover;
            theSelectedItem->setIcon(theSelectedItem->Img_hover);
        }
    }
}

void TestListWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton){
        startPos = event->pos();
    }
}

void TestListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if((event->pos() - startPos).manhattanLength() > 5) return;

    TestListWidgetItem *item = static_cast<TestListWidgetItem *>(itemAt(event->pos()));
    setCurrentItem(item);
}

void TestListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton){
        if((event->pos() - startPos).manhattanLength() < QApplication::startDragDistance()) return;

        TestListWidgetItem *theDragItem = static_cast<TestListWidgetItem *>(itemAt(startPos));
        //setCurrentItem(theDragItem);

        theDragRow = row(theDragItem);
        theSelectedRow = row(currentItem());

        update(QRect(0, 40 * theDragRow, width(), 40));

        QString text = theDragItem->text();
        QIcon icon = theDragItem->Img;
        QIcon icon_hover = theDragItem->Img_hover;
        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << text << icon << icon_hover;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData(myMimeType(), itemData);

        thumbnail *DragImage = new thumbnail(this);
        DragImage->setupthumbnail(icon_hover, text);
        //DragImage->setIconSize(18);  //default:20
        QPixmap pixmap = DragImage->grab();

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

        if(drag->exec(Qt::MoveAction) == Qt::MoveAction){
            if(theSelectedRow != theDragRow) delete takeItem(row(theDragItem));
        }
    }
}

void TestListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    TestListWidget *source = qobject_cast<TestListWidget *>(event->source());
    if (source && source == this) {
        IsDraging = true;
        emit beginDrag();
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void TestListWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    emit endDrag();
    theHighlightedRow = -2;

    if(!theHighlightedRect.contains(QRect(0, 40 * theDragRow, width(), 40))){
        update(theHighlightedRect);
    }

//this ensures DropIndicator to be disappear while dragleaving
    IsDraging = false;

    theInsertRow = -1;
    event->accept();
}

void TestListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    TestListWidget *source = qobject_cast<TestListWidget *>(event->source());
    if (source && source == this) {

        if(timeCount >= timeLimit){
            int posYToParent = event->pos().y(); //timer interval control
            if(posYToParent >= height() - scrollMargin){
                emit Interval((height() - posYToParent + 5) / 3);
            }else if(posYToParent <= scrollMargin){
                emit Interval((posYToParent + 5) / 3);
            }
        }

        oldHighlightedRect = theHighlightedRect;
        theHighlightedRect = targetRect(event->pos());

        if(event->pos().y() >= offset()){

            theHighlightedRow = row(itemAt(event->pos() - QPoint(0, offset())));

            if(oldHighlightedRect != theHighlightedRect){
                update(oldHighlightedRect);
                update(theHighlightedRect);
            }else
                update(theHighlightedRect);

            theInsertRow = row(itemAt(event->pos() - QPoint(0, offset()))) + 1;
        }else{
            theHighlightedRow = -1;
            update(QRect(0, 0, width(), 80));  //仅刷新第一行
            theInsertRow = 0;
        }

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void TestListWidget::dropEvent(QDropEvent *event)
{
    TestListWidget *source = qobject_cast<TestListWidget *>(event->source());
    if (source && source == this){

        IsDraging = false;
        emit endDrag();

        theHighlightedRow = -2;
        update(theHighlightedRect);

        if(theInsertRow == theDragRow || theInsertRow == theDragRow + 1) return;

        if(theSelectedRow == theDragRow){
            QListWidget::dropEvent(event);
            return;
        }

        QString text;
        QIcon icon, icon_hover;
        QByteArray itemData = event->mimeData()->data(myMimeType());
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        dataStream >> text >> icon >> icon_hover;

        insertItem(theInsertRow, new TestListWidgetItem(icon, icon_hover, text/*, this*/));//!parent can not be set here!

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

//void TestListWidget::startDrag(Qt::DropActions)
//{

//    QListWidgetItem *theDragItem = currentItem();
//    theDragRow = row(theDragItem);

//    QString text = theDragItem->text();
//    QIcon icon = theDragItem->icon();
//    //QIcon icon_hover = theDragItem->Img_hover;
//    QByteArray itemData;
//    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
//    dataStream << text << icon;

//    QMimeData *mimeData = new QMimeData;
//    mimeData->setData(myMimeType(), itemData);

//    thumbnail *DragImage = new thumbnail(this);
//    DragImage->setupthumbnail(icon, text);
//    //DragImage->setIconSize(18);  //default:20
//    QPixmap pixmap = DragImage->grab();

//    QDrag *drag = new QDrag(this);
//    drag->setMimeData(mimeData);
//    drag->setPixmap(pixmap);
//    drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

//    if(drag->exec(Qt::MoveAction) == Qt::MoveAction){
//        //if(theSelectedRow != theDragRow) delete takeItem(row(theDragItem));
//    }
//}

//响应子线程中QTimer的timeout()信号
void TestListWidget::doAutoScroll()
{
    QPoint pos = mapFromGlobal(QCursor::pos());
    int posYToParent = pos.y();
    int step = 1;

    if(posYToParent >= height() - scrollMargin && scrollBarValue < scrollBarRange){
        if(timeCount >= timeLimit){
            emit exceptedValue(scrollBarValue + step);
            if(pos.y() + scrollBarValue >= offset()){
                theHighlightedRow = row(itemAt(pos - QPoint(0, offset())));

                oldHighlightedRect = theHighlightedRect;
                theHighlightedRect = targetRect(pos);

                if(oldHighlightedRect != theHighlightedRect){
                    update(oldHighlightedRect);
                    update(theHighlightedRect);
                }
                theInsertRow = row(itemAt(pos - QPoint(0, offset()))) + 1;
            }
        }else{
            timeCount++;
        }
    }else if(posYToParent <= scrollMargin && scrollBarValue > 0){
        if(timeCount >= timeLimit){
            emit exceptedValue(scrollBarValue - step);
            if(pos.y() + scrollBarValue >= offset()){
                theHighlightedRow = row(itemAt(pos - QPoint(0, offset())));

                oldHighlightedRect = theHighlightedRect;
                theHighlightedRect = targetRect(pos);

                if(oldHighlightedRect != theHighlightedRect){
                    update(oldHighlightedRect);
                    update(theHighlightedRect);
                }
                theInsertRow = row(itemAt(pos - QPoint(0, offset()))) + 1;
            }else if(pos.y() + scrollBarValue >= 0){
                theHighlightedRow = -1;
                update(QRect(0, 0, width(), 80));
                theInsertRow = 0;
            }else{
                theHighlightedRow = -2;
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

bool TestListWidget::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::HoverMove:
//        oldHighlightItem = theHighlightItem;
//        theHighlightItem = static_cast<TestListWidgetItem *>(itemAt(mapFromGlobal(QCursor::pos())));

//        if(oldHighlightItem != theHighlightItem){
//            if(oldHighlightItem && !oldHighlightItem->isSelected()) {
//                oldHighlightItem->setTextColor(Color);
//                oldHighlightItem->setIcon(oldHighlightItem->Img);
//            }
//            if(theHighlightItem && !theHighlightItem->isSelected()) {
//                theHighlightItem->setTextColor(Color_hover);
//                theHighlightItem->setIcon(theHighlightItem->Img_hover);
//            }
//        }
        break;
    case QEvent::HoverLeave:
//        oldHighlightItem = theHighlightItem;
//        if(oldHighlightItem && !oldHighlightItem->isSelected()) {
//            oldHighlightItem->setTextColor(Color);
//            oldHighlightItem->setIcon(oldHighlightItem->Img);
//        }
//        oldHighlightItem = theHighlightItem = nullptr;
        break;
    default:
        break;
    }
    return QListWidget::event(event);
}

const QRect TestListWidget::targetRect(const QPoint &position) const
{
    if(position.y() >= offset())
        return QRect(0, (position.y() - offset()) / 40 * 40, width(), 2 * 40);
    else
        return QRect(0, 0, width(), 40);
}

//const QRect TestListWidget::selectedRect(const QPoint &position) const
//{
//    return QRect(0, position.y() / 40 * 40, width(), 40);
//}

TestListWidgetItem::TestListWidgetItem(QListWidget *view) :
    QListWidgetItem(view)
{
}

TestListWidgetItem::TestListWidgetItem(const QIcon &icon, const QIcon &icon_hover, QString &text, QListWidget *view) :
    QListWidgetItem(view)
{
    Img = icon;
    Img_hover = icon_hover;
    setText(text);
    setIcon(Img);
}

void TestListWidgetItem::setUpIcon(const QIcon &icon, const QIcon &icon_hover)
{
    Img = icon;
    Img_hover = icon_hover;
    setIcon(Img);
}

TestItemDelegate::TestItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void TestItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    TestListWidget *dragWidget = qobject_cast<TestListWidget *>(option.styleObject);
    bool isDraging = dragWidget->isDraging();
    //bool isWheeling = dragWidget->isWheeling();
    //int theRowByMenu = dragWidget->rowByMenu();

    QRect rect = option.rect;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

//    QPixmap Image;
//    QPixmap Image_hover;
//    Image.load(":/listBar_Icon/dir.png");
//    Image_hover.load(":/listBar_Icon/dir_hover.png");


    if(option.state & (QStyle::State_MouseOver | QStyle::State_Selected)){

        TestListWidgetItem *item = static_cast<TestListWidgetItem *>(dragWidget->item(index.row()));
        item->setIcon(item->Img_hover);

//        QStandardItemModel *listModel = qobject_cast<QStandardItemModel *>(dragWidget->model());
//        TestListViewItem *item = static_cast<TestListViewItem *>(listModel->item(index.row()));
//        item->setIcon(item->Img_hover);

        if(option.state & QStyle::State_MouseOver){
        }
        if(option.state & QStyle::State_Selected){
            painter->setBrush(QColor(180, 0, 0));
            painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), 4, rect.height());

            painter->setBrush(QColor(230, 231, 234));
            painter->drawRect(rect.topLeft().x() + 4, rect.topLeft().y(), rect.width() - 4, rect.height());

        }
    }else{
        TestListWidgetItem *item = static_cast<TestListWidgetItem *>(dragWidget->item(index.row()));
        item->setIcon(item->Img);

//        QStandardItemModel *listModel = qobject_cast<QStandardItemModel *>(dragWidget->model());
//        TestListViewItem *item = static_cast<TestListViewItem *>(listModel->item(index.row()));
//        item->setIcon(item->Img);
    }

//begin wheel
//    if(isWheeling){
//        int theWheelRow = dragWidget->wheelRow();

//        QStyleOptionViewItem opt(option);

//        if(option.state & QStyle::State_Selected){  //保证item在selected状态下不会被hover
//            opt.state &= ~QStyle::State_MouseOver;
//        }

//        if(option.state & QStyle::State_MouseOver){  //处理QScrollBar的wheel
//            opt.state &= ~QStyle::State_MouseOver;
//        }

//        if(index.row() == theWheelRow){
//            opt.state |= QStyle::State_MouseOver;
//            painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image_hover);
//        }else if(option.state & ~QStyle::State_Selected){
//            painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, Image);
//        }

//        QStyledItemDelegate::paint(painter, opt, index);
//        return;
//    }
//end wheel

//begin drag
    if(isDraging){
        int theDragRow = dragWidget->dragRow();
        int theSelectedRow = dragWidget->selectedRow();
        int UpRow = dragWidget->highlightedRow();
        int DownRow = UpRow + 1;
        int rowCount = dragWidget->model()->rowCount() - 1;

//只绘制空隙,而不绘制DropIndicator
        if(index.row() == theSelectedRow && theDragRow != theSelectedRow){
            if(index.row() == UpRow && index.row() != theDragRow - 1){
                int offset = 3;
                QPolygon trianglePolygon_bottomLeft;
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (offset + WIDTH) + 1);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (offset + WIDTH + POLYGON) + 1);
                trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x() + POLYGON, rect.bottomLeft().y() - (offset + WIDTH) + 1);

                QPolygon trianglePolygon_bottomRight;
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - (offset + WIDTH) + 1);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - (offset + WIDTH + POLYGON) + 1);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() - POLYGON + 1, rect.bottomRight().y() - (offset + WIDTH) + 1);

                painter->setBrush(QColor(245, 245, 247));
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
                trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + offset + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + offset + WIDTH + POLYGON);
                trianglePolygon_topRight << QPoint(rect.topRight().x() - POLYGON + 1, rect.topRight().y() + offset + WIDTH);

                painter->setBrush(QColor(245, 245, 247));
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
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - (POLYGON + WIDTH) + 1 - WIDTH);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - WIDTH + 1 - WIDTH);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() - POLYGON + 1, rect.bottomRight().y() - WIDTH + 1 - WIDTH);

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
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - (POLYGON + WIDTH) + 1);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - WIDTH + 1);
                trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() - POLYGON + 1, rect.bottomRight().y() - WIDTH + 1);

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
                trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + (POLYGON + WIDTH) + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + WIDTH + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x() - POLYGON + 1, rect.topRight().y() + WIDTH + WIDTH);

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
                trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + (POLYGON + WIDTH));
                trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + WIDTH);
                trianglePolygon_topRight << QPoint(rect.topRight().x() - POLYGON + 1, rect.topRight().y() + WIDTH);

                painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), rect.width(), WIDTH);  //rect
                painter->drawPolygon(trianglePolygon_topLeft);
                painter->drawPolygon(trianglePolygon_topRight);
            }
        }

//update to highlighted the theDragRow
        if(index.row() == theDragRow && theDragRow != theSelectedRow){

            painter->setBrush(QColor(180, 0, 0));
            painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), 4, rect.height());

            painter->setBrush(QColor(230, 231, 234));
            painter->drawRect(rect.topLeft().x() + 4, rect.topLeft().y(), rect.width() - 4, rect.height());

//opt.state is used to hightlight the font
            QStyleOptionViewItem opt(option);
            opt.state |= QStyle::State_Selected;
            QStyledItemDelegate::paint(painter, opt, index);

//            TestListWidgetItem *item = static_cast<TestListWidgetItem *>(dragWidget->item(index.row()));
//            item->setIcon(item->Img_hover);

            return;
        }

        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
//end drag

    QStyledItemDelegate::paint(painter, option, index);
}



TestListModel::TestListModel(QListView *parent) :
    QStandardItemModel(parent)
{
}

QVariant TestListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
        return static_cast<QStandardItem *>(item(index.row(), 0))->text();
    else if (role == Qt::DecorationRole)
        return static_cast<QStandardItem *>(item(index.row(), 0))->icon();
//    else if (role == Qt::UserRole + 1)
//        return static_cast<TestListViewItem *>(item(index.row(), 0))->Img_hover;

    return QVariant();
}

QMimeData *TestListModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            QString text = data(index, Qt::DisplayRole).toString();
            QIcon icon = qvariant_cast<QIcon>(data(index, Qt::DecorationRole));
            //QIcon icon_hover = qvariant_cast<QIcon>(data(index, Qt::UserRole + 1));
            stream << text << icon;
        }
    }

    mimeData->setData("TestListView/text-icon", encodedData);
    return mimeData;
}

//bool TestListModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
//                               int row, int column, const QModelIndex &parent)
//{
//    if (!data->hasFormat("TestListView/text-icon"))
//        return false;

//    if (action == Qt::IgnoreAction)
//        return true;

//    if (column > 0)
//        return false;

//    int endRow;

//    if (!parent.isValid()) {
//        if (row < 0)
//            endRow = rowCount();
//        else
//            endRow = qMin(row, rowCount());
//    } else {
//        endRow = parent.row();
//    }

//    QByteArray encodedData = data->data("image/x-puzzle-piece");
//    QDataStream stream(&encodedData, QIODevice::ReadOnly);

//    while (!stream.atEnd()) {
//        QString text;
//        QIcon icon, icon_hover;
//        stream >> text >> icon >> icon_hover;

//        beginInsertRows(QModelIndex(), endRow, endRow);
//        endInsertRows();

//        ++endRow;
//    }

//    return true;
//}

QStringList TestListModel::mimeTypes() const
{
    QStringList types;
    types << "TestListView/text-icon";
    return types;
}


Qt::DropActions TestListModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

TestListViewItem::TestListViewItem()
    :QStandardItem()
{
}

TestListViewItem::TestListViewItem(const QIcon &icon, const QIcon &icon_hover, const QString &text)
    :QStandardItem()
{
    Img = icon;
    Img_hover = icon_hover;
    setIcon(Img);
    setText(text);
}

TestListView::TestListView(QWidget *parent) :
    QListView(parent)
{
//    setMouseTracking(true);
//    setDragEnabled(true);
    setAcceptDrops(true);
//    setDropIndicatorShown(true);
//    setDefaultDropAction(Qt::MoveAction);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

//void TestListView::mousePressEvent(QMouseEvent *event)
//{
//    QListView::mousePressEvent(event);
//    if(event->buttons() & Qt::LeftButton){
//        startPos = event->pos();
//    }
//}

//void TestListView::mouseReleaseEvent(QMouseEvent *event)
//{
//    if((event->pos() - startPos).manhattanLength() > 5) return;

//    QModelIndex index = indexAt(event->pos());
//    setCurrentIndex(index);
//}

//void TestListView::mouseMoveEvent(QMouseEvent *event)
//{
//    if(event->buttons() & Qt::LeftButton){
//        if((event->pos() - startPos).manhattanLength() < QApplication::startDragDistance()) return;

//        QModelIndex theDragIndex = indexAt(startPos);
//        theDragRow = theDragIndex.row();
//        //theSelectedRow = currentIndex().row();

//        //setCurrentIndex(theDragIndex);
//        //emit clicked(theDragIndex);

////        QStandardItemModel *listModel = qobject_cast<QStandardItemModel *>(model());
////        TestListViewItem *theDragItem = static_cast<TestListViewItem *>(listModel->item(theDragRow));

////        QString text = theDragItem->text();
////        QIcon icon = theDragItem->Img;
////        QIcon icon_hover = theDragItem->Img_hover;
////        QByteArray itemData;
////        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
////        dataStream << text << icon << icon_hover;

////        QMimeData *mimeData = new QMimeData;
////        mimeData->setData(myMimeType(), itemData);

////        thumbnail *DragImage = new thumbnail(this);
////        DragImage->setupthumbnail(icon_hover, text);
////        //DragImage->setIconSize(18);  //default:20
////        QPixmap pixmap = DragImage->grab();

////        QDrag *drag = new QDrag(this);
////        drag->setMimeData(mimeData);
////        drag->setPixmap(pixmap);
////        drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

////        if(drag->exec(Qt::MoveAction) == Qt::MoveAction){
//////            int theRemoveRow = -1;
//////            if(theInsertRow < theDragRow) theRemoveRow = theDragRow + 1;
//////            else if(theInsertRow > theDragRow) theRemoveRow = theDragRow;
//////            model()->removeRow(theRemoveRow);
////        }

//        QStandardItemModel *listModel = qobject_cast<QStandardItemModel *>(model());
//        QStandardItem *theDragItem = listModel->item(theDragRow);

//        QString text = theDragItem->text();
//        QIcon icon = theDragItem->icon();
//        QByteArray itemData;
//        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
//        dataStream << text << icon;

//        QMimeData *mimeData = new QMimeData;
//        mimeData->setData(myMimeType(), itemData);

//        thumbnail *DragImage = new thumbnail(this);
//        DragImage->setupthumbnail(icon, text);
//        //DragImage->setIconSize(18);  //default:20
//        QPixmap pixmap = DragImage->grab();

//        QDrag *drag = new QDrag(this);
//        drag->setMimeData(mimeData);
//        drag->setPixmap(pixmap);
//        drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

//        if(drag->exec(Qt::MoveAction) == Qt::MoveAction){
//            int theRemoveRow = -1;
//            if(theInsertRow < theDragRow) theRemoveRow = theDragRow + 1;
//            else theRemoveRow = theDragRow;
//            model()->removeRow(theRemoveRow);
//        }
//    }
//}

//void TestListView::dragEnterEvent(QDragEnterEvent *event)
//{
//    TestListView *source = qobject_cast<TestListView *>(event->source());
//    if (source && source == this) {
//        IsDraging = true;
//        event->setDropAction(Qt::MoveAction);
//        event->accept();
//    }
//}

//void TestListView::dragLeaveEvent(QDragLeaveEvent *event)
//{
//    oldHighlightedRow = theHighlightedRow;
//    theHighlightedRow = -2;

//    update(model()->index(oldHighlightedRow, 0));
//    update(model()->index(oldHighlightedRow + 1, 0));

////this ensures DropIndicator to be disappear while dragleaving
//    IsDraging = false;

//    theInsertRow = -1;
//    event->accept();
//}

//void TestListView::dragMoveEvent(QDragMoveEvent *event)
//{
//    TestListView *source = qobject_cast<TestListView *>(event->source());
//    if (source && source == this) {

//        oldHighlightedRow = theHighlightedRow;
//        theHighlightedRow = indexAt(event->pos() - QPoint(0, offset())).row();

//        if(event->pos().y() >= offset()){

//            if(oldHighlightedRow != theHighlightedRow){
//                update(model()->index(oldHighlightedRow, 0));
//                update(model()->index(oldHighlightedRow + 1, 0));

//                update(model()->index(theHighlightedRow, 0));
//                update(model()->index(theHighlightedRow + 1, 0));
//            }else{
//                update(model()->index(theHighlightedRow, 0));
//                update(model()->index(theHighlightedRow + 1, 0));
//            }

//            theInsertRow = theHighlightedRow + 1;
//        }else{
//            theHighlightedRow = -1;
//            update(model()->index(0, 0));
//            update(model()->index(1, 0));
//            theInsertRow = 0;
//        }

//        event->setDropAction(Qt::MoveAction);
//        event->accept();
//    }
//}

//void TestListView::dropEvent(QDropEvent *event)
//{
//    TestListView *source = qobject_cast<TestListView *>(event->source());
//    if (source && source == this){

//        IsDraging = false;

//        oldHighlightedRow = theHighlightedRow;
//        theHighlightedRow = -2;

//        update(model()->index(oldHighlightedRow, 0));
//        update(model()->index(oldHighlightedRow + 1, 0));

//        //if(theInsertRow == theDragRow || theInsertRow == theDragRow + 1) return;

//        //if(theSelectedRow == theDragRow){
//            //QListView::dropEvent(event);
//            //return;
//        //}

//        QString text;
//        QIcon icon;
//        QByteArray itemData = event->mimeData()->data(myMimeType());
//        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
//        dataStream >> text >> icon;

//        model()->insertRow(theInsertRow);

//        QStandardItemModel *listModel = qobject_cast<QStandardItemModel *>(model());
//        //listModel->setItem(theInsertRow, 0, new TestListViewItem(icon, icon_hover, text));
//        listModel->setItem(theInsertRow, 0, new QStandardItem(icon, text));

//        setCurrentIndex(model()->index(theInsertRow, 0));

//        event->setDropAction(Qt::MoveAction);
//        event->accept();
//    }
//}

//void TestListView::startDrag(Qt::DropActions)
//{
//    theDragRow = currentIndex().row();

//    QStandardItemModel *listModel = qobject_cast<QStandardItemModel *>(model());
//    QStandardItem *theDragItem = listModel->item(theDragRow);

//    QString text = theDragItem->text();
//    QIcon icon = theDragItem->icon();
//    QByteArray itemData;
//    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
//    dataStream << text << icon;

//    QMimeData *mimeData = new QMimeData;
//    mimeData->setData(myMimeType(), itemData);

//    thumbnail *DragImage = new thumbnail(this);
//    DragImage->setupthumbnail(icon, text);
//    //DragImage->setIconSize(18);  //default:20
//    QPixmap pixmap = DragImage->grab();

//    QDrag *drag = new QDrag(this);
//    drag->setMimeData(mimeData);
//    drag->setPixmap(pixmap);
//    drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

//    if(drag->exec(Qt::MoveAction) == Qt::MoveAction){
//        int theRemoveRow = -1;
//        if(theInsertRow < theDragRow) theRemoveRow = theDragRow + 1;
//        else theRemoveRow = theDragRow;
//        model()->removeRow(theRemoveRow);
//    }
//}

TestArrowWidget::TestArrowWidget(QWidget *parent)
    : QWidget(parent)
    , m_triangleWidth(TRIANGLE_WIDTH)
    , m_triangleHeight(TRIANGLE_HEIGHT)
{
    initUi();
}

void TestArrowWidget::initUi()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint/* | Qt::Popup*/);
    setAttribute(Qt::WA_TranslucentBackground);

    setFixedSize(300, 230);
    m_startX = height() / 2 - TRIANGLE_WIDTH / 2;

    auto shadowEffect = new QGraphicsDropShadowEffect(this); //设置阴影边框;
    shadowEffect->setOffset(0, 0);
    shadowEffect->setColor(Qt::gray);
    shadowEffect->setBlurRadius(SHADOW_WIDTH);
    this->setGraphicsEffect(shadowEffect);
}

void TestArrowWidget::setStartPos(int startX)  //设置小三角显示的起始位置;
{
    m_startX = startX;
}

void TestArrowWidget::setTriangleInfo(int width, int height)
{
    m_triangleWidth = width;
    m_triangleHeight = height;
}

void TestArrowWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(230, 245, 230, 250));

    QPolygon trianglePolygon_Left;
    trianglePolygon_Left << QPoint(SHADOW_WIDTH, m_startX + m_triangleWidth / 2);
    trianglePolygon_Left << QPoint(SHADOW_WIDTH + m_triangleHeight, m_startX);
    trianglePolygon_Left << QPoint(SHADOW_WIDTH + m_triangleHeight, m_startX + m_triangleWidth);

    QPainterPath drawPath;
    drawPath.addRoundedRect(m_triangleHeight + SHADOW_WIDTH, SHADOW_WIDTH,
                            width() - SHADOW_WIDTH * 2 - m_triangleHeight,
                            height() - SHADOW_WIDTH * 2,
                            BORDER_RADIUS, BORDER_RADIUS);  //Rect

    drawPath.addPolygon(trianglePolygon_Left);  //Triangle;
    painter.drawPath(drawPath);
}

//bool TestArrowWidget::event(QEvent *event)  //消去Popup自带阴影
//{
//    static bool class_amended = false;
//    if(event->type() == QEvent::WinIdChange){
//        HWND hwnd = reinterpret_cast<HWND>(winId());
//        if (class_amended == false){
//            class_amended = true;
//            DWORD class_style = ::GetClassLong(hwnd, GCL_STYLE);
//            class_style &= ~static_cast<unsigned long>(CS_DROPSHADOW);
//            ::SetClassLong(hwnd, GCL_STYLE, static_cast<long>(class_style));
//        }
//    }else if(event->type() == QEvent::Hide){
//        emit AddDirWidgetIsHidden();
//    }
//    return QWidget::event(event);
//}
