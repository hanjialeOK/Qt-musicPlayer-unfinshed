#include "widget_Children/widget_localmusic.h"

//***********************CustomHeaderView*************************CustomHeaderView***********************CustomHeaderView*********************//
CustomHeaderView::CustomHeaderView(Qt::Orientation orientation, QWidget *parent) :
    QHeaderView(orientation, parent)
{
}

void CustomHeaderView::mouseReleaseEvent(QMouseEvent *event)
{
    QHeaderView::mouseReleaseEvent(event);
    emit HeaderViewReleased();
}

//*********************CustomScrollBar****************************CustomScrollBar************************CustomScrollBar*********************//

CustomScrollBar::CustomScrollBar(QWidget *parent) :
    QScrollBar(parent)
{
}

void CustomScrollBar::wheelEvent(QWheelEvent *event)
{
    QScrollBar::wheelEvent(event);
    emit ScrollBarWheeled();
    //update();
}

void CustomScrollBar::mouseReleaseEvent(QMouseEvent *event)
{
    QScrollBar::mouseReleaseEvent(event);
    emit ScrollBarReleased();
}

//void CustomScrollBar::mousePressEvent(QMouseEvent *event)
//{
//    QScrollBar::mousePressEvent(event);
//    double pos = static_cast<double>(event->pos().y()) / static_cast<double>(height());
////防止双击拖动handle时激发此信号
//    if(!isSliderDown()){
//        setValue(static_cast<int>(pos * (maximum() - minimum()) + minimum()));
//    }
//}

void CustomScrollBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(180, 0, 0, 200));

    int d = width() - 2;
    if(targetY >= d / 2)
        painter.drawEllipse((width() - d) / 2, targetY - d / 2, d, d);

    QScrollBar::paintEvent(event);
}

bool CustomScrollBar::event(QEvent *event)
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
    case QEvent::Enter:
        setStyleSheet(
                    "QScrollBar{background:transparent; width:10px; margin:0px 0px 0px 0px;}"
                    "QScrollBar::handle{background:rgba(205, 205, 205, 200); border:0px; border-radius:5px; margin:0px 0px 0px 0px;}"
                    "QScrollBar::handle:hover{background:rgba(205, 205, 205, 200);}"
                    "QScrollBar::handle:pressed{background:rgba(190, 190, 192, 200);}"
                    "QScrollBar::sub-page{background:transparent;}"
                    "QScrollBar::add-page{background:transparent;}"
                    "QScrollBar::up-arrow{background:transparent;}"
                    "QScrollBar::down-arrow{background:transparent;}"
                    "QScrollBar::sub-line{background:transparent; height:0px;}"
                    "QScrollBar::add-line{background:transparent; height:0px;}"
                    );
        break;
    case QEvent::Leave:
        setStyleSheet(
                    "QScrollBar{background:transparent; width:10px; margin:0px 0px 0px 0px;}"
                    "QScrollBar::handle{background:rgba(223, 223, 225, 200); border:0px; border-radius:5px; margin:0px 0px 0px 0px;}"
                    "QScrollBar::handle:hover{background:rgba(205, 205, 205, 200);}"
                    "QScrollBar::handle:pressed{background:rgba(190, 190, 192, 200);}"
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

void CustomScrollBar::getSliderRange(int min, int max)
{
    setMinimum(min);
    setMaximum(max);
    if(max == min) hide();
    else show();
}

//***********************MyTableView*****************************MyTableView*************************MyTableView**************************//

MyTableView::MyTableView(QWidget *parent)
    : QTableView (parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(false);
    setDefaultDropAction(Qt::MoveAction);
    //setDragDropMode(QAbstractItemView::DragDrop);

    setMouseTracking(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    CustomHeaderView *HeaderView = new CustomHeaderView(Qt::Horizontal, this);
    setHorizontalHeader(HeaderView);
    horizontalHeader()->installEventFilter(this);
    connect(HeaderView, &CustomHeaderView::HeaderViewReleased, this, &MyTableView::HandleHeaderViewReleased);

    scrollBarTimer = new MyTimer();
    //scrollBarTimer->setInterval(10);
    QThread *timerThread = new QThread(this);
    scrollBarTimer->moveToThread(timerThread);
    connect(this, &MyTableView::beginDrag, scrollBarTimer, &MyTimer::beginTimer);
    connect(this, &MyTableView::endDrag, scrollBarTimer, &MyTimer::stopTimer);
    connect(this, &MyTableView::Interval, scrollBarTimer, &MyTimer::getInterval);
    connect(scrollBarTimer, &MyTimer::timeout, this, &MyTableView::doAutoScroll);
    connect(timerThread, &QThread::finished, scrollBarTimer, &MyTimer::deleteLater);
    timerThread->start();
}

void MyTableView::leaveEvent(QEvent *event)
{
    QTableView::leaveEvent(event);
    curPosition = QPoint(-1, -1);

    IsTableViewEntered = false;
    IsHeaderOrBarEntered = false;
}

void MyTableView::HandleSliderReleased()
{
    if(mapFromGlobal(QCursor::pos()).x() >= width() - 10){
        return;
    }

    curPosition = mapFromGlobal(QCursor::pos()) - QPoint(0, horizontalHeader()->height());

    QModelIndex index = indexAt(curPosition);

    int oldHoverRow = mHoverRow;
    mHoverRow = index.row();

    IsTableViewEntered = true;
    if (selectionBehavior() == SelectRows && oldHoverRow != mHoverRow) {
        for (int i = 0; i < model()->columnCount(); ++i)
            update(model()->index(mHoverRow, i));
    }

    if(index.column() == 2 || index.column() == 3) update(index);
}

void MyTableView::HandleScrollBarWheeled()
{
    curPosition = mapFromGlobal(QCursor::pos()) - QPoint(0, horizontalHeader()->height());

    QModelIndex index = indexAt(curPosition);
    int oldHoverRow = mHoverRow;
    mHoverRow = index.row();

    if(flag){
        update(model()->index(index.row(), index.column()));  //每次wheel时只运行一次
        flag = false;
    }

    if (selectionBehavior() == SelectRows && oldHoverRow != mHoverRow) {
        for (int i = 0; i < model()->columnCount(); ++i){
            update(model()->index(oldHoverRow, i));
            update(model()->index(mHoverRow, i));
        }
    }

    if(index.column() == 2 || index.column() == 3) update(index);
}

void MyTableView::HandleHeaderViewReleased()
{
    curPosition = mapFromGlobal(QCursor::pos()) - QPoint(0, horizontalHeader()->height());
    QModelIndex index = indexAt(curPosition);
    int oldHoverRow = mHoverRow;
    mHoverRow = index.row();

    IsTableViewEntered = true;
    if (selectionBehavior() == SelectRows && oldHoverRow != mHoverRow) {
        for (int i = 0; i < model()->columnCount(); ++i)
            update(model()->index(mHoverRow, i));
    }

    if(index.column() == 2 || index.column() == 3) update(index);
}

void MyTableView::mousePressEvent(QMouseEvent *event)
{
    //QTableView::mousePressEvent(event);
    if(event->buttons() & Qt::LeftButton){
        startPos = event->pos();
    }
}

void MyTableView::mouseDoubleClickEvent(QMouseEvent *event)
{
    //QTableView::mouseDoubleClickEvent(event);
    if(isDoubleClickEnabled()){
        QModelIndex index = indexAt(event->pos());
        emit doubleClicked(index);

//        oldPlayingRow = thePlayingRow;
//        thePlayingRow = index.row();
//        update(model()->index(oldPlayingRow, 0));
//        update(model()->index(thePlayingRow, 0));
    }
}

void MyTableView::mouseReleaseEvent(QMouseEvent *event)
{
    QTableView::mouseReleaseEvent(event);  //is necessary in editorEvent
    QModelIndex index = indexAt(event->pos());

    if(event->button() == Qt::LeftButton){
        setCurrentIndex(index);
        emit clicked(index);
    }

    if(event->button() == Qt::RightButton){
    }
}

//setMouseTracking(true) is necessary
void MyTableView::mouseMoveEvent(QMouseEvent *event)
{
    curPosition = event->pos();

    flag = true;

    QModelIndex index = indexAt(event->pos());

    int oldHoverRow = mHoverRow;
    int oldHoverColumn = mHoverColumn;
    mHoverRow = index.row();
    mHoverColumn = index.column();

    IsTableViewEntered = true;

    if(selectionBehavior() == SelectRows && oldHoverRow != mHoverRow){
        for(int i = 0; i < model()->columnCount(); ++i){
            update(model()->index(oldHoverRow, i));
            update(model()->index(mHoverRow, i));
        }
    }
    if(selectionBehavior() == SelectColumns && oldHoverColumn != mHoverColumn){
        for(int i = 0; i < model()->rowCount(); ++i) {
            update(model()->index(i, mHoverColumn));
            update(model()->index(i, oldHoverColumn));
        }
    }

    if(index.column() == 2 || index.column() == 3) update(index);
//start drag
    if(event->buttons() & Qt::LeftButton){
        if((event->pos() - startPos).manhattanLength() < QApplication::startDragDistance()) return;

        QModelIndex theDragIndex = indexAt(startPos);
        theDragRow = theDragIndex.row();

        if(theDragIndex != indexAt(event->pos())) update(indexAt(event->pos()));

        setCurrentIndex(theDragIndex);
        emit clicked(theDragIndex);

//        QString num = model()->index(theDragRow, 0).data().toString();
//        QString title = model()->index(theDragRow, 1).data().toString();
//        QString author = model()->index(theDragRow, 2).data().toString();
//        QString album = model()->index(theDragRow, 3).data().toString();
//        QString duation = model()->index(theDragRow, 4).data().toString();
//        QString size = model()->index(theDragRow, 5).data().toString();

        QString num = model()->data(model()->index(theDragRow, 0)).toString();
        QString title = model()->data(model()->index(theDragRow, 1)).toString();
        QString author = model()->data(model()->index(theDragRow, 2), Qt::UserRole).toString();
        QString album = model()->data(model()->index(theDragRow, 3), Qt::UserRole).toString();
        QString duation = model()->data(model()->index(theDragRow, 4)).toString();
        QString size = model()->data(model()->index(theDragRow, 5)).toString();

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << num << title << author << album << duation << size;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData(myMimeType(), itemData);

        thumbnail *DragImage = new thumbnail(this);
        DragImage->setupthumbnail(QIcon(":/listBar_Icon/1_hover.png"), title);
        //DragImage->setIconSize(20);  //default:20
        QPixmap pixmap = DragImage->grab();

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));
        drag->setDragCursor(QPixmap(const_cast<const char **>(move_xpm)), Qt::MoveAction);
        connect(drag, &QDrag::destroyed, [=](){emit endDrag();});

        if(drag->exec(Qt::MoveAction) == Qt::MoveAction){
            int theRemoveRow = -1;
            if(theInsertRow < theDragRow) theRemoveRow = theDragRow + 1;
            else if(theInsertRow > theDragRow) theRemoveRow = theDragRow;
            model()->removeRow(theRemoveRow);
        }
    }
}

void MyTableView::dragEnterEvent(QDragEnterEvent *event)
{
    MyTableView *source = qobject_cast<MyTableView *>(event->source());
    if (source && source == this) {
        IsDraging = true;
        emit beginDrag();
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MyTableView::dragLeaveEvent(QDragLeaveEvent *event)
{
    IsDraging = false;
    timeCount = 0;
    oldHighlightedRow = theHighlightedRow;
    theHighlightedRow = -2;
    theInsertRow = -1;
    //emit endDrag();
    for (int i = 0; i < model()->columnCount(); ++i) {
        update(model()->index(oldHighlightedRow, i));
        update(model()->index(oldHighlightedRow + 1, i));
    }
    event->accept();
}

void MyTableView::dragMoveEvent(QDragMoveEvent *event)
{
    MyTableView *source = qobject_cast<MyTableView *>(event->source());
    if (source && source == this) {

        if(event->pos().y() >= offset()){

            oldHighlightedRow = theHighlightedRow;
            theHighlightedRow = indexAt(event->pos() - QPoint(0, offset())).row();

            theInsertRow = theHighlightedRow + 1;

            if(oldHighlightedRow != theHighlightedRow){
                for (int i = 0; i < model()->columnCount(); ++i) {
                    update(model()->index(oldHighlightedRow, i));
                    update(model()->index(oldHighlightedRow + 1, i));
                }
                for (int i = 0; i < model()->columnCount(); ++i) {
                    update(model()->index(theHighlightedRow, i));
                    update(model()->index(theHighlightedRow + 1, i));
                }
            }
        }else{
            theHighlightedRow = -1;
            theInsertRow = 0;
            for (int i = 0; i < model()->columnCount(); ++i) {
                update(model()->index(0, i));
                update(model()->index(1, i));
            }
        }

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MyTableView::dropEvent(QDropEvent *event)
{
    MyTableView *source = qobject_cast<MyTableView *>(event->source());
    if (source && source == this){

        IsDraging = false;
        timeCount = 0;
        emit endDrag();

        oldHighlightedRow = theHighlightedRow;
        theHighlightedRow = -2;

        for (int i = 0; i < model()->columnCount(); ++i) {
            update(model()->index(oldHighlightedRow, i));
            update(model()->index(oldHighlightedRow + 1, i));
        }

        if(theInsertRow == theDragRow || theInsertRow == theDragRow + 1 || theInsertRow < 0) return;

//![part 1] moveRow does not work at all!
//        for(int i = 0; i < model()->columnCount(); i++)
//            model()->moveRow(model()->index(theDragRow, i), theDragRow, model()->index(theInsertRow, i), theInsertRow);
//![part 1]

//![part 2] insert a new row and set data for each column
        QString num, title, author, album, duation, size;
        QByteArray itemData = event->mimeData()->data(myMimeType());
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        dataStream >> num >> title >> author >> album >> duation >> size;

        model()->insertRow(theInsertRow);

        model()->setData(model()->index(theInsertRow, 0), num);
        model()->setData(model()->index(theInsertRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
        model()->setData(model()->index(theInsertRow, 1), title);
        model()->setData(model()->index(theInsertRow, 2), author, Qt::UserRole);
        model()->setData(model()->index(theInsertRow, 3), album, Qt::UserRole);
        model()->setData(model()->index(theInsertRow, 4), duation);
        model()->setData(model()->index(theInsertRow, 5), size);

        if(theInsertRow > theDragRow){

            for(int i = theDragRow; i <= theInsertRow; i++){
                model()->setData(model()->index(i, 0), QString::number(i));
                model()->setData(model()->index(i, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
            }
        }else{

            for(int i = theInsertRow; i <= theDragRow; i++){
                model()->setData(model()->index(i, 0), QString::number(i + 1));
                model()->setData(model()->index(i, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
            }
        }
//![part 2]

//![part 3] update playingRow Icon. so wired the code is!😂
        if(theDragRow == thePlayingRow){
            //oldPlayingRow = thePlayingRow;
            if(theInsertRow > theDragRow)
                setPlayingRow(theInsertRow - 1);
                //thePlayingRow = theInsertRow - 1;
            else
                setPlayingRow(theInsertRow);
                //thePlayingRow = theInsertRow;

            QVariant data = model()->data(model()->index(theInsertRow, 0));
            model()->setData(model()->index(theInsertRow, 0), data, Qt::UserRole);
            model()->setData(model()->index(theInsertRow, 0), QVariant());
            update(model()->index(thePlayingRow, 0));

        }else if(theDragRow < thePlayingRow && theInsertRow > thePlayingRow){
            //oldPlayingRow = thePlayingRow;
            //thePlayingRow -= 1;
            setPlayingRow(thePlayingRow - 1);

            QVariant data = model()->data(model()->index(oldPlayingRow, 0));
            model()->setData(model()->index(oldPlayingRow, 0), data, Qt::UserRole);
            model()->setData(model()->index(oldPlayingRow, 0), QVariant());

        }else if(theDragRow > thePlayingRow && theInsertRow <= thePlayingRow){
            //oldPlayingRow = thePlayingRow;
            //thePlayingRow += 1;
            setPlayingRow(thePlayingRow + 1);

            QVariant data = model()->data(model()->index(thePlayingRow, 0));
            model()->setData(model()->index(thePlayingRow, 0), data, Qt::UserRole);
            model()->setData(model()->index(thePlayingRow, 0), QVariant());
        }
//![part 3]

        setCurrentIndex(model()->index(theInsertRow, 0));
        emit clicked(model()->index(theInsertRow, 0));
        emit dragMsg(theDragRow, theInsertRow);

//![part 4] update button delegate
        curPosition = event->pos();
        QModelIndex index = indexAt(curPosition);
        if(index.column() == 2 || index.column() == 3) update(index);
//![part 4]

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MyTableView::doAutoScroll()
{
    QPoint pos = mapFromGlobal(QCursor::pos());
    if(pos.x() < 0 || pos.x() > width()){
        timeCount = 0;
        return;
    }
    int posYToParent = mapToParent(pos).y() - scrollBarValue + 60;
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

                oldHighlightedRow = theHighlightedRow;
                theHighlightedRow = indexAt(pos - QPoint(0, horizontalHeader()->height()) - QPoint(0, offset())).row();

                if(oldHighlightedRow != theHighlightedRow){
                    for (int i = 0; i < model()->columnCount(); ++i) {
                        update(model()->index(oldHighlightedRow, i));
                        update(model()->index(oldHighlightedRow + 1, i));
                    }
                    for (int i = 0; i < model()->columnCount(); ++i) {
                        update(model()->index(theHighlightedRow, i));
                        update(model()->index(theHighlightedRow + 1, i));
                    }
                }
                theInsertRow = theHighlightedRow + 1;
            }
        }else{
            timeCount++;
        }
    }else if(posYToParent <= scrollMargin && scrollBarValue > 0){
        if(timeCount >= timeLimit){

            emit Interval((posYToParent + 5) / 3);
            emit exceptedValue(scrollBarValue - step);

            if(pos.y() >= offset()){

                oldHighlightedRow = theHighlightedRow;
                theHighlightedRow = indexAt(pos - QPoint(0, horizontalHeader()->height()) - QPoint(0, offset())).row();

                if(oldHighlightedRow != theHighlightedRow){
                    for (int i = 0; i < model()->columnCount(); ++i) {
                        update(model()->index(oldHighlightedRow, i));
                        update(model()->index(oldHighlightedRow + 1, i));
                    }
                    for (int i = 0; i < model()->columnCount(); ++i) {
                        update(model()->index(theHighlightedRow, i));
                        update(model()->index(theHighlightedRow + 1, i));
                    }
                }
                theInsertRow = theHighlightedRow + 1;
            }else if(pos.y() >= 0){
                theHighlightedRow = -1;
                for (int i = 0; i < model()->columnCount(); ++i) {
                    update(model()->index(0, i));
                    update(model()->index(1, i));
                }
                theInsertRow = 0;
            }else{
                theHighlightedRow = -2;
                for (int i = 0; i < model()->columnCount(); ++i) {
                    update(model()->index(0, i));
                }
                theInsertRow = -1;
            }
        }else{
            timeCount++;
        }
    }else{
        timeCount = 0;
    }
}

bool MyTableView::eventFilter(QObject *obj, QEvent *event)
{
    switch(event->type()){
//不能return true,否则导致hover样式失效
    case QEvent::HoverEnter:
        if(obj == horizontalHeader() || obj == verticalScrollBar()){
            IsHeaderOrBarEntered = true;
        }
        break;
    case QEvent::HoverLeave:
        if(obj == horizontalHeader() || obj == verticalScrollBar()){
            IsHeaderOrBarEntered = false;
        }
        break;
//只能检测到horizontalHeader()
    case QEvent::Leave:
        break;
//检测不到release
    case QEvent::MouseButtonRelease:
        if(obj == horizontalHeader()){
        }
        break;
    case QEvent::MouseButtonDblClick:
        break;
//只能检测到horizontalHeader()
    case QEvent::HoverMove:
        break;
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}

//************************StyledItemDelegate***********************StyledItemDelegate**********************StyledItemDelegate**************//

ViewItemDelegate::ViewItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent),
      font("Microsoft YaHei", 9, QFont::Normal),
      m_Height(22),
      m_pButton(new QPushButton())
{
    m_pButton->setStyleSheet("QPushButton{background-color:transparent; color:rgba(20, 20, 20, 230); border:0px solid rgb(0 ,0 ,0);}"
                             "QPushButton{border-bottom:2px solid transparent;}"
                             "QPushButton{font-family:Microsoft YaHei; font-size:15px; font-weight:Normal; text-align:left center;}"
                             //"QPushButton:hover{color:rgba(180, 0, 0, 255); border-bottom:2px solid rgba(180, 0, 0, 255);}"
                             "QPushButton:hover{color:rgba(0, 0, 0, 255); border-bottom:2px solid rgba(0, 0, 0, 255);}"
                             "QPushButton:pressed{color:rgba(20, 20, 20, 255);}"
                             //"QPushButton:pressed{color:rgba(180, 0, 0, 255);}"
                             );
}

void ViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    QRect rect = option.rect;

    //QColor NormalColor = index.row() % 2 ? QColor(247, 247, 250) : QColor(253, 253, 253);
    //QColor HoverColor = QColor(235, 236, 237);
    QColor SelectColor = QColor(240, 240, 240);
    QColor BorderColor = QColor(180, 0, 0);

//![1] hover entire row / wheel
    QStyleOptionViewItem opt(option);
    //opt.state &= ~QStyle::State_HasFocus;

    MyTableView *view = qobject_cast<MyTableView *>(opt.styleObject);
    QTableView::SelectionBehavior behavior = view->selectionBehavior();
    QModelIndex hoverIndex = view->hoverIndex();
    bool HoverEnterTableView = view->isTableViewEntered();
    bool HoverEnterHeaderOrBar = view->isHeaderOrBarEntered();

    if(option.state & QStyle::State_MouseOver){  //处理QScrollBar的wheelEvent
        opt.state &= ~QStyle::State_MouseOver;
    }

    if (!(option.state & QStyle::State_Selected) && behavior != QTableView::SelectItems
            && (!HoverEnterHeaderOrBar && HoverEnterTableView)) {
        if (behavior == QTableView::SelectRows && hoverIndex.row() == index.row()){
            opt.state |= QStyle::State_MouseOver;
        }
        if (behavior == QTableView::SelectColumns && hoverIndex.column() == index.column())
            opt.state |= QStyle::State_MouseOver;
    }

    if(HoverEnterHeaderOrBar || !HoverEnterTableView){
        opt.state &= ~QStyle::State_MouseOver;
    }
    QStyledItemDelegate::paint(painter, opt, index);
//![1]

//! [2] update selected style  ************  update selected style  **********  update selected style//
    int rowCount = view->model()->rowCount() - 1;
    int columnCount = view->model()->columnCount() - 1;

    if(option.state & QStyle::State_Selected){
        QPen borderPen;
        borderPen.setColor(BorderColor);
        borderPen.setWidth(2);
        int radius = 4;

        if(index.column() == 0){
            QPainterPath leftRectPath;
            leftRectPath.moveTo(rect.topRight()/* + QPoint(0, 1)*/ + QPoint(1, 0));
            leftRectPath.lineTo(rect.topLeft() + QPoint(radius, 0)/* + QPoint(0, 1)*/);
            leftRectPath.arcTo(QRect(rect.topLeft()/* + QPoint(1, 1)*/, QSize(radius * 2, radius * 2)), 90, 90);
            leftRectPath.lineTo(rect.bottomLeft() - QPoint(0, radius) + QPoint(0, 1));
            leftRectPath.arcTo(QRect(rect.bottomLeft() - QPoint(0, radius * 2)/* + QPoint(1, 0)*/ + QPoint(0, 1), QSize(radius * 2, radius * 2)), 180, 90);
            leftRectPath.lineTo(rect.bottomRight() + QPoint(1, 0) + QPoint(0, 1));
            leftRectPath.lineTo(rect.topRight()/* + QPoint(0, 1)*/ + QPoint(1, 0));

            painter->setPen(Qt::NoPen);
            painter->setBrush(SelectColor);
            painter->drawPath(leftRectPath);

            QPainterPath leftBorderPath;
            leftBorderPath.moveTo(rect.topRight() + QPoint(0, 1));
            leftBorderPath.lineTo(rect.topLeft() + QPoint(radius, 0) + QPoint(0, 1));
            leftBorderPath.arcTo(QRect(rect.topLeft() + QPoint(1, 1), QSize(radius * 2, radius * 2)), 90, 90);
            leftBorderPath.lineTo(rect.bottomLeft() - QPoint(0, radius) + QPoint(1, 1));
            leftBorderPath.arcTo(QRect(rect.bottomLeft() - QPoint(0, radius * 2) + QPoint(1, 0), QSize(radius * 2, radius * 2)), 180, 90);
            leftBorderPath.lineTo(rect.bottomRight());

            painter->setPen(borderPen);
            painter->drawPath(leftBorderPath);

        }else if(index.column() == columnCount){
            QPainterPath rightRectPath;
            rightRectPath.moveTo(rect.bottomLeft() + QPoint(0, 1));
            rightRectPath.arcTo(QRect(rect.bottomRight() - QPoint(radius * 2, radius * 2) + QPoint(1, 1), QSize(radius * 2, radius * 2)), 270, 90);
            rightRectPath.lineTo(rect.topRight() + QPoint(0, radius) + QPoint(1, 0));
            rightRectPath.arcTo(QRect(rect.topRight() - QPoint(radius * 2, 0) + QPoint(1, 0), QSize(radius * 2, radius * 2)), 0, 90);
            rightRectPath.lineTo(rect.topLeft()/* + QPoint(0, 1)*/);
            rightRectPath.lineTo(rect.bottomLeft() + QPoint(0, 1));

            painter->setPen(Qt::NoPen);
            painter->setBrush(SelectColor);
            painter->drawPath(rightRectPath);

            QPainterPath rightBorderPath;
            rightBorderPath.moveTo(rect.bottomLeft());
            rightBorderPath.arcTo(QRect(rect.bottomRight() - QPoint(radius * 2, radius * 2), QSize(radius * 2, radius * 2)), 270, 90);
            rightBorderPath.lineTo(rect.topRight() + QPoint(0, radius) + QPoint(0, 1));
            rightBorderPath.arcTo(QRect(rect.topRight() - QPoint(radius * 2, 0) + QPoint(0 ,1), QSize(radius * 2, radius * 2)), 0, 90);
            rightBorderPath.lineTo(rect.topLeft() + QPoint(0, 1));

            painter->setPen(borderPen);
            painter->drawPath(rightBorderPath);

//            painter->drawLine(rect.topLeft() + QPoint(0, 1), rect.topRight() - QPoint(radius, 0) + QPoint(0, 1));
//            painter->drawLine(rect.bottomLeft(), rect.bottomRight() - QPoint(radius, 0));

//            painter->drawLine(rect.topRight() + QPoint(0, radius) + QPoint(0, 1), rect.bottomRight() - QPoint(0, radius) + QPoint(0, 1));

//            painter->drawArc(QRect(rect.topRight() - QPoint(radius, 0) + QPoint(0 ,1), QSize(radius, radius)), 0 * 16, 90 * 16);
//            painter->drawArc(QRect(rect.bottomRight() - QPoint(radius, radius), QSize(radius, radius)), 270 * 16, 90 * 16);
        }else{
            painter->setPen(Qt::NoPen);
            painter->setBrush(SelectColor);
            painter->drawRect(rect);

            painter->setPen(borderPen);
            painter->drawLine(rect.topLeft() + QPoint(0, 1), rect.topRight() + QPoint(0, 1));
            painter->drawLine(rect.bottomLeft(), rect.bottomRight());
        }
        QStyledItemDelegate::paint(painter, option, index);
        //return;
    }else if(option.state & QStyle::State_MouseOver){

    }
//! [2]

//! [2.5] ItemDelegateForColumn  ************  ItemDelegateForColumn  **********  ItemDelegateForColumn//

    if(index.column() == 0){
        if(index.row() == view->playingRow()){
            int IconSize = 20;
            int X = (rect.width() - IconSize) / 2;
            int Y = (rect.height() - IconSize) / 2;
            QPixmap pixmap_play;
            QPixmap pixmap_pause;
            pixmap_play.load(QStringLiteral(":/localmusic_Icon/play.png"));
            pixmap_pause.load(QStringLiteral(":/localmusic_Icon/pause.png"));

            if(view->state() == QMediaPlayer::PlayingState)
                painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, pixmap_play);
            else
                painter->drawPixmap(rect.topLeft().x() + X, rect.topLeft().y() + Y, IconSize, IconSize, pixmap_pause);
        }
    }

    QApplication::restoreOverrideCursor();
    if(index.column() == 2 || index.column() == 3){
        QString author = index.model()->data(index, Qt::UserRole).toString();

        int textWidth = QFontMetrics(font).width(author);
        int m_Width;
        QString text;
        if(textWidth >= rect.width() - 10){
            m_Width = rect.width() - 10;
            text = QFontMetrics(font).elidedText(author, Qt::ElideRight, rect.width() - 10);
        }else{
            m_Width = textWidth;
            text = author;
        }

        int X = 3;
        int Y = (rect.height() - m_Height) / 2 + 1; //2 is for border
        QRect buttonRect = QRect(rect.topLeft() + QPoint(X, Y), QSize(m_Width, m_Height));

        QStyleOptionButton button;
        button.text = text;
        button.rect = buttonRect;
        button.state |= QStyle::State_Enabled;

//            if(option.state & QStyle::State_Selected){
//                button.state |= QStyle::State_Selected;
//            }
//            else if(option.state & QStyle::State_MouseOver){
//                button.state |= QStyle::State_MouseOver;
//            }else{
//                button.state |= QStyle::State_None;
//            }

        if(option.state & QStyle::State_Selected){
            button.state |= QStyle::State_Sunken;
        }

        if(/*option.state & QStyle::State_MouseOver && */buttonRect.contains(view->curPos())){  //wheel
            QApplication::setOverrideCursor(Qt::PointingHandCursor);
            button.state |= QStyle::State_MouseOver;
        }

        m_pButton.data()->style()->drawControl(QStyle::CE_PushButton, &button, painter, m_pButton.data());
    }

//! [2.5]

//! [3] drag
    if(view->isDraging()){
        //QStyledItemDelegate::paint(painter, option, index);  //必须放在前面,否则偶数行会影响绘图

        int theDragRow = view->dragRow();
        int UpRow = view->highlightedRow();
        int DownRow = UpRow + 1;

        if(index.row() == UpRow && index.row() != theDragRow - 1 && index.row() != theDragRow){
            painter->setBrush(QColor(66, 133, 244));
            if(UpRow == rowCount){
                //到达尾部,三角形向上移动一个WIDTH的距离,以使分隔符宽度*2
                if(index.column() == 0){
                    QPolygon trianglePolygon_bottomLeft;
                    trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (POLYGON + WIDTH) + 1 - WIDTH);
                    trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1 - WIDTH);
                    trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x() + POLYGON, rect.bottomLeft().y() - WIDTH + 1 - WIDTH);
                    painter->drawPolygon(trianglePolygon_bottomLeft);
                }else if(index.column() == columnCount){
                    QPolygon trianglePolygon_bottomRight;
                    trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - (POLYGON + WIDTH) + 1 - WIDTH);
                    trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - WIDTH + 1 - WIDTH);
                    trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1 - POLYGON, rect.bottomRight().y() - WIDTH + 1 - WIDTH);
                    painter->drawPolygon(trianglePolygon_bottomRight);
                }
                painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - 2 * WIDTH + 1, rect.width(), 2 * WIDTH);  //rect
            }else {
                //正常情况,组成上半部分(+1是根据实际情况修正)
                if(index.column() == 0){
                    QPolygon trianglePolygon_bottomLeft;
                    trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (POLYGON + WIDTH) + 1);
                    trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1);
                    trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x() + POLYGON, rect.bottomLeft().y() - WIDTH + 1);
                    painter->drawPolygon(trianglePolygon_bottomLeft);
                }else if(index.column() == columnCount){
                    QPolygon trianglePolygon_bottomRight;
                    trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - (POLYGON + WIDTH) + 1);
                    trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - WIDTH + 1);
                    trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1 - POLYGON, rect.bottomRight().y() - WIDTH + 1);
                    painter->drawPolygon(trianglePolygon_bottomRight);
                }
                painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1, rect.width(), WIDTH);  //rect
            }
        }
        else if(index.row() == DownRow && index.row() != theDragRow + 1 && index.row() != theDragRow){
            painter->setBrush(QColor(66, 133, 244));
            if(DownRow == 0){
                //到达头部,三角形向下移动一个WIDTH的距离,以使分隔符宽度*2
                if(index.column() == 0){
                    QPolygon trianglePolygon_topLeft;
                    trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + (POLYGON + WIDTH) + WIDTH);
                    trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + WIDTH + WIDTH);
                    trianglePolygon_topLeft << QPoint(rect.topLeft().x() + POLYGON, rect.topLeft().y() + WIDTH + WIDTH);
                    painter->drawPolygon(trianglePolygon_topLeft);
                }else if(index.column() == columnCount){
                    QPolygon trianglePolygon_topRight;
                    trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + (POLYGON + WIDTH) + WIDTH);
                    trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + WIDTH + WIDTH);
                    trianglePolygon_topRight << QPoint(rect.topRight().x() + 1 - POLYGON, rect.topRight().y() + WIDTH + WIDTH);
                    painter->drawPolygon(trianglePolygon_topRight);
                }
                painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), rect.width(), 2 * WIDTH);  //rect
            }
            else {
                //正常情况
                if(index.column() == 0){
                    QPolygon trianglePolygon_topLeft;
                    trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + (POLYGON + WIDTH));
                    trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + WIDTH);
                    trianglePolygon_topLeft << QPoint(rect.topLeft().x() + POLYGON, rect.topLeft().y() + WIDTH);
                    painter->drawPolygon(trianglePolygon_topLeft);
                }else if(index.column() == columnCount){
                    QPolygon trianglePolygon_topRight;
                    trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + (POLYGON + WIDTH));
                    trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + WIDTH);
                    trianglePolygon_topRight << QPoint(rect.topRight().x() + 1 - POLYGON, rect.topRight().y() + WIDTH);
                    painter->drawPolygon(trianglePolygon_topRight);
                }
                painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), rect.width(), WIDTH);  //rect
            }
        }
        return;
    }
//! [3]
}

bool ViewItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    MyTableView *view = qobject_cast<MyTableView *>(option.styleObject);
    view->setDoubleClickEnabled(true);

    if(index.column() != 2 && index.column() != 3) return false;

    state = Normal;
    QMouseEvent *pEvent = static_cast<QMouseEvent *> (event);
    QPoint pos = pEvent->pos();

    QApplication::restoreOverrideCursor();

    QString text = model->data(index, Qt::UserRole).toString();
    QRect rect = option.rect;
    int textWidth = QFontMetrics(font).width(text);
    int m_Width;
    if(textWidth >= rect.width() - 10){
        m_Width = rect.width() - 10;
    }else{
        m_Width = textWidth;
    }
    int X = 3;
    int Y = (rect.height() - m_Height) / 2 + 1;
    QRect buttonRect = QRect(rect.topLeft() + QPoint(X, Y), QSize(m_Width, m_Height));

    if(buttonRect.contains(pos)){
        switch (event->type()){
        case QEvent::MouseButtonPress:
            qDebug()<<text<<"is pressed";
            break;
        case QEvent::MouseButtonRelease:
            qDebug()<<text<<"is released";
            if(index.column() == 2){

            }else if(index.column() == 3){

            }
            view->setDoubleClickEnabled(false);
            break;
        default:
            break;
        }
    }
    return true;

    //return QStyledItemDelegate::editorEvent(event, model, option, index);
}

//*****************************TreeView***************************TreeView******************************TreeView***************************//

TreeView::TreeView(QWidget *parent)
    : QTreeView (parent)
{
}

void TreeView::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}
//*************************LocalMusic类******************************LocalMusic类***************************LocalMusic类*************************//

LocalMusic::LocalMusic(QWidget *parent) :
    QWidget(parent)
{
    initUi();
}

//LocalMusic::~LocalMusic()
//{
//    delete LocalMusicButton;
//    delete AddDirectoryButton;
//    delete SongNumberLabel;
//}

void LocalMusic::initUi()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    resize(1130 + 18 - 250, 750 + 47 - 120);
    setMaximumSize(1920, 1030);

    LocalMusicButton = new QPushButton(this);
    LocalMusicButton->setFixedSize(120, 30);
    LocalMusicButton->setIconSize(QSize(30, 30));
    LocalMusicButton->setIcon(QIcon(QStringLiteral(":/localmusic_Icon/quit.png")));
    LocalMusicButton->setText("本地音乐");
    LocalMusicButton->setFont(QFont("Microsoft YaHei", 14, QFont::Normal));
    LocalMusicButton->setStyleSheet("QPushButton{border:0px solid rgb(0, 0, 0); color:rgba(0, 0, 0, 255);}"
                                    "QPushButton{text-align:left bottom; padding-left:-8px;}"
                                    //"QPushButton:hover{color:rgba(0, 0, 0, 255);}"
                                    );
    LocalMusicButton->setFocusPolicy(Qt::NoFocus);
    LocalMusicButton->setCursor(Qt::PointingHandCursor);
    connect(LocalMusicButton, &QPushButton::clicked, this, &LocalMusic::close);

    SongNumberLabel = new QPushButton(this);
    SongNumberLabel->setFixedSize(65, 30);
    SongNumberLabel->setText(QString::number(0) + "首音乐 ,");
    SongNumberLabel->setFont(QFont("Microsoft YaHei", 9, QFont::Normal));
    SongNumberLabel->setStyleSheet("QPushButton{border:0px solid rgb(0, 0, 0); text-align:left bottom;}");
    SongNumberLabel->setFocusPolicy(Qt::NoFocus);

    AddDirectoryButton = new QPushButton(this);
    AddDirectoryButton->setFixedSize(65, 30);
    AddDirectoryButton->setText(" 选择目录");
    AddDirectoryButton->setFont(QFont("Microsoft YaHei", 9, QFont::Normal));
    AddDirectoryButton->setStyleSheet("QPushButton{border:0px solid rgb(0, 0, 0); color:rgba(48, 136, 203, 200); text-align:left bottom;}"
                                      "QPushButton:checked{color:rgba(48, 136, 203, 255);}");
    AddDirectoryButton->setFocusPolicy(Qt::NoFocus);
    AddDirectoryButton->setCursor(Qt::PointingHandCursor);

    musicTabButton = new QPushButton(this);
    musicTabButton->setFixedSize(34, 29);
    musicTabButton->setCheckable(true);
    musicTabButton->setIconSize(QSize(23, 23));
    musicTabButton->setIcon(QIcon(QStringLiteral(":/localmusic_Icon/list.png")));
    musicTabButton->setStyleSheet("QPushButton{background-color:rgb(251, 251, 251); border:1px solid rgb(200, 200, 200);}"
                                  "QPushButton{border-top-left-radius:4px; border-bottom-left-radius:4px;}"
                                  "QPushButton:hover{background-color:rgb(220, 220, 220);}"
                                  "QPushButton:checked{background-color:rgb(124, 125, 133); border:solid rgb(124, 125, 133);}");
    musicTabButton->setFocusPolicy(Qt::NoFocus);
    musicTabButton->setCursor(Qt::PointingHandCursor);

    authorTabButton = new QPushButton(this);
    authorTabButton->setFixedSize(32, 29);
    authorTabButton->setCheckable(true);
    authorTabButton->setIconSize(QSize(16, 16));
    authorTabButton->setIcon(QIcon(":/localmusic_Icon/author.png"));
    authorTabButton->setStyleSheet("QPushButton{background-color:rgb(251, 251, 251); border-top:1px solid rgb(200, 200, 200); border-bottom:1px solid rgb(200, 200, 200);}"
                                   "QPushButton:hover{background-color:rgb(220, 220, 220);}"
                                   "QPushButton:checked{background-color:rgb(124, 125, 133); border:solid rgb(124, 125, 133);}");
    authorTabButton->setFocusPolicy(Qt::NoFocus);
    authorTabButton->setCursor(Qt::PointingHandCursor);

    ablumTabButton = new QPushButton(this);
    ablumTabButton->setFixedSize(32, 29);
    ablumTabButton->setCheckable(true);
    ablumTabButton->setIconSize(QSize(16, 16));
    ablumTabButton->setIcon(QIcon(":/localmusic_Icon/ablum.png"));
    ablumTabButton->setStyleSheet("QPushButton{background-color:rgb(251, 251, 251); border:1px solid rgb(200, 200, 200); border-right:0px;}"
                                  "QPushButton:hover{background-color:rgb(220, 220, 220);}"
                                  "QPushButton:checked{background-color:rgb(124, 125, 133); border:solid rgb(124, 125, 133);}");
    ablumTabButton->setFocusPolicy(Qt::NoFocus);
    ablumTabButton->setCursor(Qt::PointingHandCursor);

    dirTabButton = new QPushButton(this);
    dirTabButton->setFixedSize(34, 29);
    dirTabButton->setCheckable(true);
    dirTabButton->setIconSize(QSize(16, 16));
    dirTabButton->setIcon(QIcon(":/localmusic_Icon/dir.png"));
    dirTabButton->setStyleSheet("QPushButton{background-color:rgb(251, 251, 251); border:1px solid rgb(200, 200, 200);}"
                                "QPushButton{border-top-right-radius:4px; border-bottom-right-radius:4px;}"
                                "QPushButton:hover{background-color:rgb(220, 220, 220);}"
                                "QPushButton:checked{background-color:rgb(124, 125, 133); border:solid rgb(124, 125, 133);}");
    dirTabButton->setFocusPolicy(Qt::NoFocus);
    dirTabButton->setCursor(Qt::PointingHandCursor);

    QButtonGroup *tabButtonGroup = new QButtonGroup(this);
    tabButtonGroup->addButton(musicTabButton, 0);
    tabButtonGroup->addButton(authorTabButton, 1);
    tabButtonGroup->addButton(ablumTabButton, 2);
    tabButtonGroup->addButton(dirTabButton, 3);
    tabButtonGroup->setExclusive(true);
    connect(tabButtonGroup, static_cast<void(QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled), this, &LocalMusic::updateTabButtonIcon);
//    connect(musicTabButton, &QPushButton::toggled, this, &LocalMusic::updateTabButtonIcon);
//    connect(authorTabButton, &QPushButton::toggled, this, &LocalMusic::updateTabButtonIcon);
//    connect(ablumTabButton, &QPushButton::toggled, this, &LocalMusic::updateTabButtonIcon);
//    connect(dirTabButton, &QPushButton::toggled, this, &LocalMusic::updateTabButtonIcon);

    QHBoxLayout *grandchildLayout = new QHBoxLayout();
    grandchildLayout->setSpacing(0);
    grandchildLayout->addSpacing(20);
    grandchildLayout->addWidget(LocalMusicButton);
    grandchildLayout->addSpacing(20);
    grandchildLayout->addWidget(SongNumberLabel);
    //grandchildLayout->addSpacing(2);
    grandchildLayout->addWidget(AddDirectoryButton);
    grandchildLayout->addStretch();
    grandchildLayout->addWidget(musicTabButton);
    grandchildLayout->addWidget(authorTabButton);
    grandchildLayout->addWidget(ablumTabButton);
    grandchildLayout->addWidget(dirTabButton);
    grandchildLayout->setContentsMargins(0, 23, 40, 7);

    playallButton = new QPushButton(this);
    playallButton->setFixedSize(100, 30);
    playallButton->setIconSize(QSize(17, 17));
    playallButton->setIcon(QIcon(":/localmusic_Icon/playall.png"));
    playallButton->setText("播放全部");
    playallButton->setFont(QFont("微软雅黑", 9, QFont::Medium));
    playallButton->setStyleSheet(
                "QPushButton{border-right:1px solid rgb(200, 200, 200); background-color:rgb(198, 47, 47); color:white;}"
                "QPushButton{border-top-left-radius:4px; border-bottom-left-radius:4px;}"
                "QPushButton:hover{background-color:rgb(180, 0, 0);}"
                );
    playallButton->setFocusPolicy(Qt::NoFocus);
    playallButton->setCursor(Qt::PointingHandCursor);

    addtoplayButton = new QPushButton(this);
    addtoplayButton->setFixedSize(35, 30);
    addtoplayButton->setIconSize(QSize(21, 21));
    addtoplayButton->setIcon(QIcon(":/localmusic_Icon/addtoplay.png"));
    addtoplayButton->setToolTip("添加全部到播放列表");
    addtoplayButton->setStyleSheet(
                "QPushButton{border:0px solid rgb(200, 200, 200); background-color:rgb(198, 47, 47);}"
                "QPushButton{border-top-right-radius:4px; border-bottom-right-radius:4px;}"
                "QPushButton:hover{background-color:rgb(180, 0, 0);}"
                );
    addtoplayButton->setFocusPolicy(Qt::NoFocus);
    addtoplayButton->setCursor(Qt::PointingHandCursor);

    matchButton = new QPushButton(this);
    matchButton->setFixedSize(90, 30);
    matchButton->setIconSize(QSize(19, 19));
    matchButton->setIcon(QIcon(":/localmusic_Icon/match.png"));
    matchButton->setText("匹配音乐");
    matchButton->setFont(QFont("微软雅黑", 9, QFont::Medium));
    matchButton->setStyleSheet(
                "QPushButton{border:1px solid rgb(200, 200, 200); background-color:rgb(255, 255, 255); color:rgba(50, 50, 50, 255);}"
                "QPushButton{border-radius:4px;}"
                "QPushButton:hover{background-color:rgb(240, 240, 240); color:rgba(0, 0, 0, 255);}"
                );
    matchButton->setFocusPolicy(Qt::NoFocus);
    matchButton->setCursor(Qt::PointingHandCursor);
    matchButton->installEventFilter(this);

    SearchForMusic = new QLineEdit(this);
    SearchForMusic->setFixedSize(200, 30);
    SearchForMusic->setPlaceholderText(QStringLiteral("搜索本地音乐"));
    SearchForMusic->setTextMargins(10, 0, 0, 0);
    SearchForMusic->setFont(QFont("微软雅黑", 9, QFont::Medium));
    SearchForMusic->addAction(QIcon(":/localmusic_Icon/search.png"), QLineEdit::TrailingPosition);
    SearchForMusic->setStyleSheet(
                "QLineEdit{background-color:transparent; color:rgba(50, 50, 50, 150);}"
                "QLineEdit{border:1px solid rgb(200, 200, 200); border-radius:15px;}"
                );

    QHBoxLayout *grandsonLayout_1 = new QHBoxLayout();
    grandsonLayout_1->addWidget(playallButton);
    grandsonLayout_1->setSpacing(0);
    grandsonLayout_1->addWidget(addtoplayButton);
    grandsonLayout_1->addSpacing(20);
    grandsonLayout_1->addWidget(matchButton);
    grandsonLayout_1->addStretch();
    grandsonLayout_1->addWidget(SearchForMusic);
    grandsonLayout_1->setContentsMargins(40, 20, 40, 20);

    SearchForDir = new QLineEdit(this);
    SearchForDir->setFixedSize(198, 30);
    SearchForDir->setPlaceholderText(QStringLiteral(" 查找文件夹"));
    SearchForDir->setFont(QFont("微软雅黑", 9, QFont::Medium));
    SearchForDir->addAction(QIcon(":/localmusic_Icon/search.png"), QLineEdit::TrailingPosition);
    SearchForDir->setStyleSheet(
                "QLineEdit{background-color:transparent; color:rgba(50, 50, 50, 150);}"
                "QLineEdit{border:1px solid rgb(200, 200, 200); border-radius:4px;}"
                );

    QVBoxLayout *grandsonLayout_2 = new QVBoxLayout();
    grandsonLayout_2->addWidget(SearchForDir);
    grandsonLayout_2->setSpacing(0);
    grandsonLayout_2->setContentsMargins(10, 20, 10, 20);

    playallButton_forDir = new QPushButton(this);
    playallButton_forDir->setFixedSize(100, 30);
    playallButton_forDir->setIconSize(QSize(17, 17));
    playallButton_forDir->setIcon(QIcon(":/localmusic_Icon/playall.png"));
    playallButton_forDir->setText("播放全部");
    playallButton_forDir->setFont(QFont("微软雅黑", 9, QFont::Medium));
    playallButton_forDir->setStyleSheet(
                "QPushButton{border-right:1px solid rgb(200, 200, 200); background-color:rgb(198, 47, 47); color:white;}"
                "QPushButton{border-top-left-radius:4px; border-bottom-left-radius:4px; padding:5px;}"
                "QPushButton:hover{background-color:rgb(180, 0, 0);}"
                );
    playallButton_forDir->setFocusPolicy(Qt::NoFocus);
    playallButton_forDir->setCursor(Qt::PointingHandCursor);

    addtoplayButton_forDir = new QPushButton(this);
    addtoplayButton_forDir->setFixedSize(35, 30);
    addtoplayButton_forDir->setIconSize(QSize(21, 21));
    addtoplayButton_forDir->setIcon(QIcon(":/localmusic_Icon/addtoplay.png"));
    addtoplayButton_forDir->setToolTip("添加全部到播放列表");
    addtoplayButton_forDir->setStyleSheet(
                "QPushButton{border:0px solid rgb(200, 200, 200); background-color:rgb(198, 47, 47);}"
                "QPushButton{border-top-right-radius:4px; border-bottom-right-radius:4px;}"
                "QPushButton:hover{background-color:rgb(180, 0, 0);}"
                );
    addtoplayButton_forDir->setFocusPolicy(Qt::NoFocus);
    addtoplayButton_forDir->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *grandsonLayout_3 = new QHBoxLayout();
    grandsonLayout_3->addSpacing(40);
    grandsonLayout_3->addWidget(playallButton_forDir);
    grandsonLayout_3->setSpacing(0);
    grandsonLayout_3->addWidget(addtoplayButton_forDir);
    grandsonLayout_3->addStretch();
    grandsonLayout_3->setContentsMargins(0, 20, 0, 20);

    TreeView_model = new QStandardItemModel();
    TreeView_model->setObjectName("TreeView_model");
    TreeView_model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("全部音乐")));
    TreeView_model->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    DirectoryTree = new TreeView(this);
    DirectoryTree->setObjectName("DirectoryTree");
    DirectoryTree->resize(210, 100);
    DirectoryTree->setMinimumWidth(70);
    DirectoryTree->setMaximumWidth(218);
    DirectoryTree->setModel(TreeView_model);
    DirectoryTree->header()->setFont(QFont("宋体", 10, QFont::DemiBold));
    DirectoryTree->header()->setSortIndicatorShown(false);  //隐藏三角符
    DirectoryTree->header()->setStyleSheet(
                "QHeaderView::section{border:0px; background-color:rgb(253, 253, 253);}"  //去掉分隔符
                "QHeaderView{color:rgb(0, 0, 0, 230);}"
                );
    DirectoryTree->setEditTriggers(QAbstractItemView::NoEditTriggers);  //不可编辑
    DirectoryTree->setContextMenuPolicy(Qt::CustomContextMenu);  //允许右键菜单
    DirectoryTree->setFrameShape(QFrame::NoFrame);  //隐藏边框
    DirectoryTree->setFocusPolicy(Qt::NoFocus);  //除去虚线
    DirectoryTree->setWordWrap(false);  //禁止自定换行
    DirectoryTree->setStyle(QStyleFactory::create("windows")); //显示默认节点间虚线
    DirectoryTree->setSelectionMode(QAbstractItemView::SingleSelection);  //单选模式
    DirectoryTree->setFont(QFont("微软雅黑", 9, QFont::Medium));
    DirectoryTree->setColumnWidth(0, 160);
    DirectoryTree->setStyleSheet(
                //"QTreeView::branch{background:palette(base);}"  //这会导致虚线不显示
                "QTreeView{border-top:1px solid rgb(200, 200, 200);}"
                "QTreeView{show-decoration-selected:1;}"
                //"QTreeView{font-size:9px; color:rgb{0, 0, 0}; background-color:rgb(253, 253, 253);}"
                "QTreeView::item{background-color:rgb(255, 255, 255); color:rgba(50, 50, 50, 200); margin-bottom:5px; height:25px;}"
                "QTreeView::item:hover{background-color:rgb(240, 240, 240); color:rgba(0, 0, 0, 240);}"
                "QTreeView::item:selected{background-color:rgb(220, 220, 220); color:rgba(0, 0, 0, 240);}"
                //"QTreeView::item:selected:active{background-color:rgb(220, 220, 220); color:rgba(0, 0, 0, 240);}"
                );
    DirectoryTree->horizontalScrollBar()->setStyleSheet(
                "QScrollBar{background:transparent; width:10px; margin:0px 0px 0px 0px;}"
                "QScrollBar::handle{background:rgb(220, 220, 220); border:0px solid transparent; border-radius:5px; margin:0px 0px 0px 0px;}"
                "QScrollBar::handle:hover{background:lightgray;}"
                "QScrollBar::sub-line{background:transparent;}"
                "QScrollBar::add-line{background:transparent;}");

    songlistModel = new QStandardItemModel();
    songlistModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("")));
    songlistModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("音乐标题")));
    songlistModel->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("歌手")));
    songlistModel->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("专辑")));
    songlistModel->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("时长")));
    songlistModel->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("大小")));
    songlistModel->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    songlistModel->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    songlistModel->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    songlistModel->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    songlistModel->horizontalHeaderItem(5)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    songlistModel_1 = new QStandardItemModel();
    songlistModel_1->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("")));
    songlistModel_1->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("音乐标题")));
    songlistModel_1->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("歌手")));
    songlistModel_1->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("专辑")));
    songlistModel_1->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("时长")));
    songlistModel_1->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("大小")));
    songlistModel_1->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    songlistModel_1->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    songlistModel_1->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    songlistModel_1->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    songlistModel_1->horizontalHeaderItem(5)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    localMusicView = new MyTableView(this);
    localMusicView->resize(1130 + 18 - 250, 36);
    localMusicView->setMinimumWidth(1130 + 18 - 250);
    localMusicView->setFixedHeight(36);
    localMusicView->setModel(songlistModel);

    localMusicView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //localMusicView->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中
    localMusicView->setSelectionMode(QAbstractItemView::ExtendedSelection);  //设置可多选
    localMusicView->setEditTriggers(QAbstractItemView::NoEditTriggers);  //表格不可编辑
    localMusicView->setContextMenuPolicy(Qt::CustomContextMenu);  //允许右键菜单
    localMusicView->setAlternatingRowColors(true);  //奇偶行颜色设置
    localMusicView->setFrameShape(QFrame::NoFrame);  //隐藏边框
    localMusicView->setFocusPolicy(Qt::NoFocus);  //去除焦点
    localMusicView->setTextElideMode(Qt::ElideRight);  //文字超长检测
    //localMusicView->setAutoFillBackground(true);
    localMusicView->setWordWrap(false);  //禁止自动换行
    localMusicView->setShowGrid(false);  //隐藏网格线
    localMusicView->verticalHeader()->setObjectName("vHeader");
    localMusicView->verticalHeader()->setDefaultSectionSize(34);  //每行默认行高
    localMusicView->verticalHeader()->hide();
    localMusicView->horizontalHeader()->setObjectName("hHeader");
    localMusicView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    localMusicView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    localMusicView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    localMusicView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    localMusicView->horizontalHeader()->setFixedHeight(36);  //设置水平行头高度
    localMusicView->horizontalHeader()->setFont(QFont("Microsoft YaHei", 9, QFont::Normal));
    localMusicView->horizontalHeader()->setSectionsClickable(false);  //表头不可点击
    localMusicView->horizontalHeader()->setStyleSheet(
                "QHeaderView{border-top:1px solid rgb(200, 200, 200); border-bottom:1px solid rgb(200, 200, 200); border-left:0px; border-right:0px;}"
                "QHeaderView{color:rgba(100, 100, 100, 230); background-color:rgb(253, 253, 253); text-align:left center;}"
                );
    localMusicView->setFont(QFont("Microsoft YaHei", 9, QFont::Normal));
    localMusicView->setColumnWidth(0, 70);
    localMusicView->setColumnWidth(1, 170);
    localMusicView->setColumnWidth(2, 170);
    localMusicView->setColumnWidth(3, 150);
    localMusicView->setColumnWidth(4, 65);
    localMusicView->setColumnWidth(5, 65);
    localMusicView->setStyleSheet(
                //QHeaderView的样式表也可以写在这里,前提是setObjectName("hHeader")
                //"QHeaderView#hHeader{border-top:1px solid rgb(200, 200, 200); border-bottom:1px solid rgb(200, 200, 200);}"
                //"QHeaderView#hHeader{color:rgba(100, 100, 100, 230); background-color:rgb(253, 253, 253); text-align:left center;}"
                "QTableView{background-color:rgb(253, 253, 253); color:rgba(20, 20, 20, 230);}"
                //"QTableView{alternate-background-color:rgb(245, 245, 247); color:rgba(50, 50, 50, 230);};"
                //"QTableView{selection-background-color:rgb(210, 210, 210); selection-color:rgba(180, 0, 0, 255);}"
                "QTableView::item:hover{background-color:rgb(235, 236, 237); /*color:rgba(10, 10, 10, 255);*/}"
                "QTableView::item:selected{background-color:transparent; /*color:rgba(180, 0, 0, 255);*/ color:rgba(20, 20, 20, 255);}"
                "QTableView{alternate-background-color:rgb(247, 247, 250); color:rgba(20, 20, 20, 230);};"
                );
    //StyledItemDelegate *delegate = new StyledItemDelegate();
    localMusicView->setItemDelegate(new ViewItemDelegate());
    connect(this, &LocalMusic::limitedHeight, localMusicView, &MyTableView::getLimitedHeight);
    connect(localMusicView, &MyTableView::doubleClicked, this, &LocalMusic::on_TabView_DoubleClicked);
    connect(localMusicView, &MyTableView::dragMsg, this, &LocalMusic::updateMpegInfoList);
    connect(localMusicView, &MyTableView::targetRow, [=](int row){
        //int rowHeight = localMusicView->verticalHeader()->defaultSectionSize();
        emit targetPosY(tabWidget->pos().y()
                        + 70
                        + localMusicView->horizontalHeader()->height()
                        + row * localMusicView->rowHeight()
                        + localMusicView->rowHeight() / 2);
    });

    MusicViewFromDir = new MyTableView(this);
    //MusicViewFromDir->setItemDelegate(delegate);
    MusicViewFromDir->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    MusicViewFromDir->resize(700, 100);
    MusicViewFromDir->setMinimumWidth(680);
    MusicViewFromDir->setMaximumWidth(800);
    MusicViewFromDir->setModel(songlistModel_1);
    MusicViewFromDir->horizontalHeader()->setObjectName("hHeader_1");
    MusicViewFromDir->verticalHeader()->setObjectName("vHeader_1");
    MusicViewFromDir->verticalHeader()->hide();
    MusicViewFromDir->setMouseTracking(true);
    MusicViewFromDir->setSelectionBehavior(QAbstractItemView::SelectRows);  //行选中
    MusicViewFromDir->setSelectionMode(QAbstractItemView::ExtendedSelection);  //设置可多选
    MusicViewFromDir->setEditTriggers(QAbstractItemView::NoEditTriggers);  //表格不可编辑
    MusicViewFromDir->setContextMenuPolicy(Qt::CustomContextMenu);  //允许右键菜单
    //MusicViewFromDir->setAlternatingRowColors(true);  //奇偶行颜色设置
    MusicViewFromDir->setFrameShape(QFrame::NoFrame);  //隐藏边框
    MusicViewFromDir->setFocusPolicy(Qt::NoFocus);
    MusicViewFromDir->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    MusicViewFromDir->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    MusicViewFromDir->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    MusicViewFromDir->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    MusicViewFromDir->horizontalHeader()->setFixedHeight(36);  //设置水平行头高度
    MusicViewFromDir->horizontalHeader()->setFont(QFont("宋体", 10, QFont::DemiBold));
    MusicViewFromDir->horizontalHeader()->setSectionsClickable(true);  //表头不可点击
    MusicViewFromDir->verticalHeader()->setDefaultSectionSize(34);  //每行默认行高
    MusicViewFromDir->setTextElideMode(Qt::ElideRight);  //文字超长检测
    MusicViewFromDir->setWordWrap(false);  //禁止自动换行
    MusicViewFromDir->setShowGrid(false);  //隐藏网格线
    MusicViewFromDir->setFont(QFont("微软雅黑", 9, QFont::Medium));
    MusicViewFromDir->setColumnWidth(0, 50);
    MusicViewFromDir->setColumnWidth(1, 180);
    MusicViewFromDir->setColumnWidth(2, 180);
    MusicViewFromDir->setColumnWidth(3, 150);
    MusicViewFromDir->setColumnWidth(4, 60);
    MusicViewFromDir->setColumnWidth(5, 60);
    MusicViewFromDir->setStyleSheet(
                "QHeaderView#hHeader_1{border-top:1px solid rgb(200, 200, 200); border-bottom:1px solid rgb(200, 200, 200);}"
                "QHeaderView#hHeader_1{color:rgba(100, 100, 100, 230); background-color:rgb(253, 253, 253); text-align:left center;}"
                "QTableView{background-color:rgb(255, 255, 255); color:rgba(50, 50, 50, 230);}"
                //"QTableView{alternate-background-color:rgb(245, 245, 247); color:rgba(50, 50, 50, 230);};"
                "QTableView{selection-background-color:rgb(220, 220, 220); selection-color:rgba(0, 0, 0, 255);}"
                "QTableView::item:hover{background-color:rgb(235, 236, 237); color:rgba(0, 0, 0, 230);}"
                //"QTableView::item:selected{background-color:rgb(200, 200, 200); color:rgba(0, 0, 0, 255);}"
                //"QTableView::item:selected:active{background:rgb(200, 200, 200); color:rgba(0, 0, 0, 255);}"
                );
    MusicViewFromDir->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);  //设置为平稳滑动

    CustomScrollBar *ScrollBarFromDir = new CustomScrollBar();
    connect(ScrollBarFromDir, &CustomScrollBar::ScrollBarWheeled, MusicViewFromDir, &MyTableView::HandleScrollBarWheeled);
    connect(ScrollBarFromDir, &CustomScrollBar::ScrollBarReleased, MusicViewFromDir, &MyTableView::HandleSliderReleased);

    QWidget *childWidget_2 = new QWidget();
    QVBoxLayout *sonLayout_2 = new QVBoxLayout(childWidget_2);
    sonLayout_2->setSpacing(0);
    sonLayout_2->addLayout(grandsonLayout_2);
    sonLayout_2->addWidget(DirectoryTree);
    sonLayout_2->setContentsMargins(0, 0, 0, 0);
    childWidget_2->setLayout(sonLayout_2);
    childWidget_2->setMaximumWidth(218);

    QWidget *childWidget_3 = new QWidget();
    QVBoxLayout *sonLayout_3 = new QVBoxLayout(childWidget_3);
    sonLayout_3->setSpacing(0);
    sonLayout_3->addLayout(grandsonLayout_3);
    sonLayout_3->addWidget(MusicViewFromDir);
    sonLayout_3->addStretch();
    sonLayout_3->setContentsMargins(0, 0, 0, 0);
    childWidget_3->setLayout(sonLayout_3);
    //childWidget_3->setMaximumWidth(680);

    QPalette painter = palette();  //设置标题栏颜色
    painter.setColor(QPalette::Background, QColor(255, 255, 255));
    childWidget_3->setAutoFillBackground(true);
    childWidget_3->setPalette(painter);

    QScrollArea *ScrollAreaFromDir = new QScrollArea(this);
    ScrollAreaFromDir->setFocusPolicy(Qt::NoFocus);  //禁止键盘方向键影响
    ScrollAreaFromDir->setFrameShape(QFrame::NoFrame);
    ScrollAreaFromDir->setVerticalScrollBar(ScrollBarFromDir);
    ScrollAreaFromDir->horizontalScrollBar()->setEnabled(false);
    ScrollAreaFromDir->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ScrollAreaFromDir->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ScrollAreaFromDir->setWidget(childWidget_3);
    ScrollAreaFromDir->setWidgetResizable(true);

    QSplitter *Splitter = new QSplitter(Qt::Orientation::Horizontal, this);
    Splitter->setHandleWidth(1);
    Splitter->setStyleSheet("QSplitter::handle{background-color:rgb(200, 200, 200);}");
    Splitter->addWidget(childWidget_2);
    Splitter->addWidget(ScrollAreaFromDir);
//    Splitter->setStretchFactor(1, 1);
//    Splitter->setStretchFactor(1, 2);

    QWidget *childWidget_1 = new QWidget();
    QVBoxLayout *sonLayout_1 = new QVBoxLayout(childWidget_1);
    sonLayout_1->addLayout(grandsonLayout_1);
    sonLayout_1->setSpacing(0);
    sonLayout_1->addWidget(localMusicView);
    sonLayout_1->addStretch();
    sonLayout_1->setContentsMargins(0, 0, 0, 0);
    childWidget_1->setLayout(sonLayout_1);

    tabWidget = new QTabWidget(this);
    tabWidget->resize(750 + 47 - 250, 1030);
    tabWidget->setFocusPolicy(Qt::NoFocus);
    tabWidget->addTab(childWidget_1, "歌曲列表");
    tabWidget->addTab(Splitter, "文件目录");
    tabWidget->setTabShape(QTabWidget::Triangular);
    tabWidget->setStyleSheet(
                "QTabWidget::pane{border-top:1px solid rgb(200, 200, 200);}"
                "QTabWidget::tab-bar{alignment:center;}"
                );
    tabWidget->tabBar()->setCursor(Qt::PointingHandCursor);
    tabWidget->setCurrentIndex(0);
    tabWidget->tabBar()->setContentsMargins(0, 0, 0, 0);
//    tabWidget->tabBar()->setStyleSheet(
//                "QTabBar::tab{height:30px; width:40px;}"
//                "QTabBar::tab{border:1px solid rgb(0, 0, 0);}"
//                "QTabBar::tab{background-color:rgb(255, 255, 255);}"
//                "QTabBar::tab:hover{background-color:rgb(240, 240, 240);}"
//                "QTabBar::tab:selected{background-color:rgb(240, 240, 240);}"
//                );
    tabWidget->tabBar()->hide();

    QVBoxLayout *Layout = new QVBoxLayout();
    Layout->setSpacing(0);
    //Layout->addSpacing(25);
    Layout->addLayout(grandchildLayout);
    Layout->addWidget(tabWidget);
    Layout->setContentsMargins(0, 0, 0, 0);
    setLayout(Layout);
    mainWidget = new QWidget(this);
    mainWidget->setLayout(Layout);
    mainWidget->resize(width(), height());

    theShownScrollBar = new CustomScrollBar(mainWidget);
    //theShownScrollBar->setSingleStep(-1);
    theShownScrollBar->setStyleSheet(
                "QScrollBar{background:transparent; width:10px; margin:0px 0px 0px 0px;}"
                "QScrollBar::handle{background:rgba(223, 223, 225, 200); border:0px; border-radius:5px; margin:0px 0px 0px 0px;}"
                "QScrollBar::handle:hover{background:rgba(205, 205, 205, 200);}"
                "QScrollBar::handle:pressed{background:rgba(190, 190, 192, 200);}"
                "QScrollBar::sub-page{background:transparent;}"
                "QScrollBar::add-page{background:transparent;}"
                "QScrollBar::up-arrow{background:transparent;}"
                "QScrollBar::down-arrow{background:transparent;}"
                "QScrollBar::sub-line{background:transparent; height:0px;}"
                "QScrollBar::add-line{background:transparent; height:0px;}"
                );
    //connect(theShownScrollBar, &CustomScrollBar::ScrollBarWheeled, localMusicView, &MyTableView::HandleScrollBarWheeled);
    connect(theShownScrollBar, &CustomScrollBar::ScrollBarReleased, localMusicView, &MyTableView::HandleSliderReleased);
    connect(theShownScrollBar, &CustomScrollBar::valueChanged, localMusicView, &MyTableView::getScrollBarValue);
    connect(theShownScrollBar, &CustomScrollBar::rangeChanged, localMusicView, &MyTableView::getscrollBarRange);
    connect(localMusicView, &MyTableView::exceptedValue, theShownScrollBar, &CustomScrollBar::setValue);
    connect(this, &LocalMusic::targetPosY, theShownScrollBar, &CustomScrollBar::getTargetY);

    ScrollArea = new QScrollArea(this);
    ScrollArea->setFocusPolicy(Qt::NoFocus);  //禁止键盘方向键影响
    ScrollArea->setFrameShape(QFrame::NoFrame);
    CustomScrollBar *theHidedScrollBar = new CustomScrollBar(ScrollArea);
    ScrollArea->setVerticalScrollBar(theHidedScrollBar);
    ScrollArea->horizontalScrollBar()->setDisabled(true);
    ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //mainWidget->setAutoFillBackground(true);
    ScrollArea->setWidget(mainWidget);

    connect(theHidedScrollBar, &CustomScrollBar::ScrollBarWheeled, localMusicView, &MyTableView::HandleScrollBarWheeled);
    connect(theHidedScrollBar, &CustomScrollBar::rangeChanged, theShownScrollBar, &CustomScrollBar::getSliderRange);
    connect(theHidedScrollBar, &CustomScrollBar::valueChanged, theShownScrollBar, &CustomScrollBar::setValue);
    connect(theShownScrollBar, &CustomScrollBar::valueChanged, theHidedScrollBar, &CustomScrollBar::setValue);

    QHBoxLayout *fatherLayout = new QHBoxLayout();
    fatherLayout->setSpacing(0);
    fatherLayout->addWidget(ScrollArea);
    fatherLayout->addSpacing(-10);
    fatherLayout->addWidget(theShownScrollBar);
    fatherLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(fatherLayout);

//    connect(LocalMusicButton, &QPushButton::clicked, this, &LocalMusic::close);
    connect(AddDirectoryButton, &QPushButton::clicked, this, &LocalMusic::on_AddDirectoryButton_Clicked);
    connect(MusicViewFromDir, &QTableView::doubleClicked, this, &LocalMusic::on_TabView_DoubleClicked);
    connect(MusicViewFromDir, &QTableView::clicked, this, &LocalMusic::selectedRowChanged);
    connect(DirectoryTree, &TreeView::clicked, this, &LocalMusic::on_treeViewClicked);
    connect(tabWidget, &QTabWidget::tabBarClicked, this, &LocalMusic::on_tabBarClicked);

    QPalette pal(palette());  //设置标题栏颜色
    pal.setColor(QPalette::Background, QColor(250, 250, 250));
    setAutoFillBackground(true);
    setPalette(pal);

    musicTabButton->setChecked(true);

    qDebug()<<localMusicView->horizontalHeader()->pos();
}

bool LocalMusic::eventFilter(QObject *obj, QEvent *event)
{
    switch(event->type()){
//检测高度变化不准确,使用resizeEvent(QResizeEvent *event)
//    case QEvent::Resize:
//        break;
//检测鼠标移动
//    case QEvent::HoverMove:
//        break;
    case QEvent::HoverEnter:
        if(obj == matchButton){
            matchButton->setIcon(QIcon(":/localmusic_Icon/match_hover.png"));
            return true;
        }
        break;
    case QEvent::HoverLeave:
        if(obj == matchButton){
            matchButton->setIcon(QIcon(":/localmusic_Icon/match.png"));
            return true;
        }
        break;
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}

void LocalMusic::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    mainWidget->setFixedWidth(event->size().width());
    if(tabWidget->currentIndex() == 1){
        mainWidget->setFixedHeight(event->size().height());
    }
    MusicViewFromDir->setMaximumWidth(width() - 70);
    emit limitedHeight(event->size().height());
}

void LocalMusic::updateTabButtonIcon(int, bool)
{
    if(musicTabButton->isChecked()){
        musicTabButton->setIcon(QIcon(":/localmusic_Icon/list_checked.png"));
        tabWidget->setCurrentIndex(0);
    }else musicTabButton->setIcon(QIcon(":/localmusic_Icon/list.png"));

    if(authorTabButton->isChecked()) authorTabButton->setIcon(QIcon(":/localmusic_Icon/author_checked.png"));
    else authorTabButton->setIcon(QIcon(":/localmusic_Icon/author.png"));

    if(ablumTabButton->isChecked()) ablumTabButton->setIcon(QIcon(":/localmusic_Icon/ablum_checked.png"));
    else ablumTabButton->setIcon(QIcon(":/localmusic_Icon/ablum.png"));

    if(dirTabButton->isChecked()){
        dirTabButton->setIcon(QIcon(":/localmusic_Icon/dir_checked.png"));
        tabWidget->setCurrentIndex(1);
    }else dirTabButton->setIcon(QIcon(":/localmusic_Icon/dir.png"));
}

void LocalMusic::updateMpegInfoList(int dragRow, int insertRow)
{
    if(insertRow > dragRow)
        AllSongs.move(dragRow, insertRow - 1);
    else
        AllSongs.move(dragRow, insertRow);

    //emit dragMsg(dragRow, insertRow);

    //for(int i = 0; i < AllSongs.size(); i++) qDebug()<<AllSongs.at(i).Title;
}

void LocalMusic::on_AddDirectoryButton_Clicked()
{
    QStringList path = QFileDialog::getOpenFileNames(this, "选择歌曲", "/", "(*.mp3)");
    if(path.isEmpty()){
        qDebug()<<"未选中任何文件!";
        return;
    }
    static int i = 0, j = 0;
    QString addSongs;
    QListIterator<QString> mylist(path);

    if(!m_whiteList.contains(QFileInfo(path.first()).path())){
        m_whiteList.append(QFileInfo(path.first()).path());
        TreeView_model->setItem(j, 0, new QStandardItem(QFileInfo(path.first()).path()));
        //TreeView_model->item(j)->setSizeHint(QSize(40, 180));
        if(selectedDir < 0){
            selectedDir = 0;
            DirectoryTree->setCurrentIndex(TreeView_model->index(0, 0));
        }
        j++;
    }

    if(!SongsSortedFromDir.contains(QFileInfo(path.first()).path())){
        QList<MpegHelper::MP3_INFO> QHashHelper;
        SongsSortedFromDir.insert(QFileInfo(path.first()).path(), QHashHelper);
    }
    QList<MpegHelper::MP3_INFO> QHashHelper = SongsSortedFromDir.find(QFileInfo(path.first()).path()).value();

    while(mylist.hasNext())
    {
        addSongs = mylist.next();
        QFileInfo fileInfo = QFileInfo(addSongs);
        QString fileName = fileInfo.fileName();
        MpegHelper::MP3_INFO tempSongInfo;

        if(!m_whiteList.contains(fileName)){
            m_whiteList.append(fileName);

//            FILE *fp = fopen(fileInfo.filePath().toUtf8().data(), "r");
//            if(fp == nullptr) qDebug()<<"11111";

            TagLib::MPEG::File *mpegFile = new TagLib::MPEG::File(QFile::encodeName(fileInfo.filePath()).constData());
            //qDebug()<<QFile::encodeName(fileInfo.filePath()).constData();
            //fileInfo.filePath().toStdString();

//            TagLib::FileRef mpegFile(fileInfo.filePath().toStdString().c_str());
//            TagLib::Tag *tag = mpegFile.tag();
//            qDebug()<<QString::fromUtf8(tag->artist().toCString(true));

            if(!mpegFile->isOpen()){
                //qDebug()<<QFile::encodeName(fileInfo.filePath()).constData();
                qDebug()<<"mpegFile cannot open!";
                continue;
            }
            tempSongInfo.isEmpty = false;
            tempSongInfo.Path = fileInfo.path();
            tempSongInfo.Url = QUrl::fromLocalFile(addSongs);
            tempSongInfo.Title = QString(mpegFile->tag()->title().toCString(true));
            tempSongInfo.Artist = QString(mpegFile->tag()->artist().toCString(true));
            tempSongInfo.Album = QString(mpegFile->tag()->album().toCString(true));
            tempSongInfo.Duration = mpegFile->audioProperties()->length();
            tempSongInfo.str_Size = QString::number(static_cast<double>(fileInfo.size())/1024/1024, 'f', 1) + 'M';

            auto tag = mpegFile->ID3v2Tag(false);
            auto frameList = tag->frameListMap()["APIC"];
            if(!frameList.isEmpty()) {
                auto frame = frameList.front();
                auto pic = reinterpret_cast<TagLib::ID3v2::AttachedPictureFrame *>(frame);
                if(pic && !pic->picture().isNull()){
                    if(tempSongInfo.Pictrue.loadFromData(
                                QByteArray::fromRawData(pic->picture().data(), static_cast<int>(pic->picture().size())))) {
                        //qDebug()<<"读取MP3封面信息成功";
                    }
                }
            }
            else{
                tempSongInfo.Pictrue.load(":/musicInfoBar_Icon/unknown.png");
                qDebug()<<"读取音乐封面信息失败";
            }
            mpegFile->~File();

            QTime totalTime((tempSongInfo.Duration / 3600) % 60, (tempSongInfo.Duration / 60) % 60,
                             tempSongInfo.Duration % 60, (tempSongInfo.Duration * 1000) % 1000);
            QString format = "mm:ss";
            if (tempSongInfo.Duration > 3600)
                format = "hh:mm:ss";
            tempSongInfo.str_Duration = totalTime.toString(format);

            if(tempSongInfo.Title.isEmpty()){
                tempSongInfo.Title = fileName;
            }
            if(tempSongInfo.Artist.isEmpty()){
                tempSongInfo.Artist = "未知作者";
            }
            if(tempSongInfo.Album.isEmpty()){
                tempSongInfo.Album = "未知专辑";
            }

            QHashHelper.append(tempSongInfo);
            AllSongs.append(tempSongInfo);
            //emit songInfo(fileInfo.path(), tempSongInfo);

//            songlistModel->setItem(i, 0, new QStandardItem(QString::number(i + 1)));
//            songlistModel->item(i, 0)->setTextAlignment(Qt::AlignCenter);
//            songlistModel->setItem(i, 1, new QStandardItem(tempSongInfo.Title));
//            //songlistModel->item(i, 1)->setToolTip(tempSongInfo.Title);
//            songlistModel->setItem(i, 2, new QStandardItem(tempSongInfo.Artist));
//            //songlistModel->item(i, 2)->setToolTip(tempSongInfo.Artist);
//            songlistModel->setItem(i, 3, new QStandardItem(tempSongInfo.Album));
//            //songlistModel->item(i, 3)->setToolTip(tempSongInfo.Album);
//            songlistModel->setItem(i, 4, new QStandardItem(tempSongInfo.str_Duration));
//            songlistModel->setItem(i, 5, new QStandardItem(tempSongInfo.str_Size));

            songlistModel->insertRow(i);
            songlistModel->setData(songlistModel->index(i, 0), QVariant(QString::number(i + 1)));
            songlistModel->setData(songlistModel->index(i, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
            songlistModel->setData(songlistModel->index(i, 1), QVariant(tempSongInfo.Title));
            songlistModel->setData(songlistModel->index(i, 2), QVariant(tempSongInfo.Artist), Qt::UserRole);
            //songlistModel->setData(songlistModel->index(i, 2), QColor(0 ,0, 0, 0), Qt::TextColorRole);
            songlistModel->setData(songlistModel->index(i, 3), QVariant(tempSongInfo.Album), Qt::UserRole);
            //songlistModel->setData(songlistModel->index(i, 3), QVariant(QPixmap::fromImage(tempSongInfo.Pictrue)), Qt::UserRole);
            songlistModel->setData(songlistModel->index(i, 4), QVariant(tempSongInfo.str_Duration));
            songlistModel->setData(songlistModel->index(i, 5), QVariant(tempSongInfo.str_Size));

            i++;
        }
    }
    SongsSortedFromDir.find(QFileInfo(path.first()).path()).value().swap(QHashHelper);

    localMusicView->setFixedHeight(localMusicView->horizontalHeader()->height()
                                   + songlistModel->rowCount() * 34 + 1);

    if(tabWidget->currentIndex() == 0){
        mainWidget->setFixedHeight(tabWidget->pos().y()  //60
                                   + 70
                                   + localMusicView->horizontalHeader()->height()
                                   + songlistModel->rowCount() * 34 + 1);
    }

//    if(tabWidget->currentIndex() == 1){
//        UpdateDataForDir(selectedDir);
//        if(!CurrentMp3Info.isEmpty){
//            UpdatePlayingRow_1(UpdateIndexForDir(CurrentMp3Info)); //刷新MusicViewFromDir图标
//            if(selectedDir == DirForSelectedRow){
//                MusicViewFromDir->setCurrentIndex(songlistModel_1->index(SelectedRow, 0)); //还原MusicViewFromDir选中行
//            }
//        }
//    }

    QString SongNumber = QString::number(i) + "首音乐 ,";
    SongNumberLabel->setText(SongNumber);
    int fontSize = SongNumberLabel->fontMetrics().width(SongNumber);  //字符超长检测
    if(fontSize >= SongNumberLabel->width()){
        SongNumberLabel->setFixedWidth(fontSize);
    }
}

void LocalMusic::on_tabBarClicked(int index)
{
    if(index == 0){
        locallistflag = -1;
        ScrollArea->verticalScrollBar()->setEnabled(true);
        ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        mainWidget->setFixedHeight(tabWidget->pos().y()
                                   + 70
                                   + localMusicView->height());
    }
    else if(index == 1){
        locallistflag = selectedDir;
        mainWidget->setFixedHeight(height());
        ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ScrollArea->verticalScrollBar()->setEnabled(false);
        if(songlistModel_1->rowCount() == 0){
            updateDataForDir(selectedDir);
            updatePlayingRow_1(updateIndexForDir(CurrentMp3Info)); //刷新MusicViewFromDir图标
        }
    }
}

void LocalMusic::on_treeViewClicked(QModelIndex index)
{
    if(selectedDir != index.row()){
        selectedDir = index.row();
        playingRow_1 = -1;
        updateDataForDir(selectedDir);
        updatePlayingRow_1(updateIndexForDir(CurrentMp3Info)); //刷新MusicViewFromDir图标
        if(selectedDir == DirForSelectedRow){
            MusicViewFromDir->setCurrentIndex(songlistModel_1->index(SelectedRow, 0));  //还原MusicViewFromDir选中行
        }
    }
}

void LocalMusic::on_TabView_DoubleClicked(QModelIndex index)
{
    if(locallistflag < 0){
        emit mediaChanged(index.row());
        return;
    }
    QList<MpegHelper::MP3_INFO> playlist = SongsSortedFromDir.find(TreeView_model->item(selectedDir)->text()).value();
    emit mediaChanged(index.row());
}

void LocalMusic::selectedRowChanged(QModelIndex index)  //记录MusicViewFromDir选中行
{
    DirForSelectedRow = selectedDir;
    SelectedRow = index.row();
}

void LocalMusic::positionJumped(MpegHelper::MP3_INFO mp3Info, int index, int flag)
{
    CurrentMp3Info = mp3Info;
    if(flag == -1){
        updatePlayingRow(index);
        updatePlayingRow_1(updateIndexForDir(mp3Info)); //刷新MusicViewFromDir图标
    }
    else{
        updatePlayingRow_1(index);
        updatePlayingRow(updateIndex(mp3Info)); //刷新localMusicView图标
    }
}

void LocalMusic::updatePlayingRow(int row)
{
    if(row < 0) return;

//    oldPlayingRow = thePlayingRow;
//    thePlayingRow = row;
    localMusicView->setPlayingRow(row);

    int oldPlayingRow = localMusicView->playedRow();
    int thePlayingRow = localMusicView->playingRow();

    QAbstractItemModel *model = localMusicView->model();

    if(oldPlayingRow >= 0){
        QVariant data = model->data(model->index(oldPlayingRow, 0), Qt::UserRole);
        model->setData(model->index(oldPlayingRow, 0), data);
        model->setData(model->index(oldPlayingRow, 0), QVariant(), Qt::UserRole);
        localMusicView->update(model->index(oldPlayingRow, 0));
    }

    QVariant data = model->data(model->index(thePlayingRow, 0));
    model->setData(model->index(thePlayingRow, 0), data, Qt::UserRole);
    model->setData(model->index(thePlayingRow, 0), QVariant());
    localMusicView->update(model->index(thePlayingRow, 0));
}

void LocalMusic::updatePlayingRow_1(int /*index*/)
{
}

void LocalMusic::updateDataForDir(int index)
{
    if(index < 0){
        return;
    }
    QList<MpegHelper::MP3_INFO> ViewFromDir = SongsSortedFromDir.find(TreeView_model->item(index)->text()).value();
    if(ViewFromDir.isEmpty()){
        return;
    }
    songlistModel_1->removeRows(0, songlistModel_1->rowCount());
    for(int i = 0; i < ViewFromDir.size(); i++){
        songlistModel_1->setItem(i, 0, new QStandardItem(QString::number(i + 1)));
        songlistModel_1->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        songlistModel_1->setItem(i, 1, new QStandardItem(ViewFromDir.at(i).Title));
        songlistModel_1->item(i, 1)->setToolTip(ViewFromDir.at(i).Title);
        songlistModel_1->setItem(i, 2, new QStandardItem(ViewFromDir.at(i).Artist));
        songlistModel_1->item(i, 2)->setToolTip(ViewFromDir.at(i).Artist);
        songlistModel_1->setItem(i, 3, new QStandardItem(ViewFromDir.at(i).Album));
        songlistModel_1->item(i, 3)->setToolTip(ViewFromDir.at(i).Album);
        songlistModel_1->setItem(i, 4, new QStandardItem(ViewFromDir.at(i).str_Duration));
        songlistModel_1->setItem(i, 5, new QStandardItem(ViewFromDir.at(i).str_Size));

        if(i % 2){
            for(int k = 0; k <= 5; k++){
                songlistModel_1->item(i, k)->setBackground(QBrush(QColor(245, 245, 247)));
            }
        }
    }
    MusicViewFromDir->setFixedHeight(MusicViewFromDir->horizontalHeader()->height()
                                     + songlistModel_1->rowCount() * 34);
}

int LocalMusic::updateIndexForDir(MpegHelper::MP3_INFO mp3Info)
{
    if(songlistModel_1->rowCount() == 0){
        return -1;
    }
    for(int i = 0; i < songlistModel_1->rowCount(); i++){
        if(songlistModel_1->item(i, 1)->text() == mp3Info.Title){
            return i;
        }
    }
    return -1;
}

int LocalMusic::updateIndex(MpegHelper::MP3_INFO mp3Info)
{
    if(songlistModel->rowCount() == 0){
        return -1;
    }
    for(int i = 0; i < songlistModel->rowCount(); i++){
        if(songlistModel->item(i, 1)->text() == mp3Info.Title){
            return i;
        }
    }
    return -1;
}

void LocalMusic::setState(QMediaPlayer::State state)
{
    if (state != playerState) {
        playerState = state;
        localMusicView->setState(state);
        localMusicView->update(localMusicView->model()->index(localMusicView->playingRow(), 0));
    }

}

void LocalMusic::on_indexIcon_Clicked()
{
    switch (state()) {
    case QMediaPlayer::StoppedState:
        emit play();
        break;
    case QMediaPlayer::PlayingState:
        emit pause();
        break;
    case QMediaPlayer::PausedState:
        emit play();
        break;
    }
}


