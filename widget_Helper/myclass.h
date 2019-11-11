#ifndef MYCLASS_H
#define MYCLASS_H

#include <QWidget>
#include <QPushButton>
#include <QEvent>
#include <QListWidget>
#include <QMouseEvent>
#include <QDebug>
#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStyledItemDelegate>
#include <QDrag>
#include <QMimeData>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QThread>

#include "widget_Helper/draganddrop.h"

class TestListWidgetItem;

class MyPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyPushButton(QWidget *parent = nullptr);
    void setUpIcon(const QIcon &icon, const QIcon &icon_hover);

protected:
    bool event(QEvent *event) override;

private:
    QIcon Img;
    QIcon Img_hover;
};

class TestListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit TestListWidget(QWidget *parent = nullptr);

    bool isDraging() const {return IsDraging;}
    int offset() const {return 19;}
    int highlightedRow() const {return theHighlightedRow;}
    int dragRow() const {return theDragRow;}
    int selectedRow() const {return theSelectedRow;}
    static QString myMimeType() { return QStringLiteral("MyListWidget/text_icon"); }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    //void startDrag(Qt::DropActions supportedActions) override;
    bool event(QEvent *event) override;

private:
    QPoint startPos;

    bool IsDraging = false;
    QRect oldHighlightedRect;
    QRect theHighlightedRect;
    int theHighlightedRow = -1;
    int theDragRow = -1;
    int theSelectedRow = -1;
    int theInsertRow = -1;

//    int limitedHeight;
    const int scrollMargin = 25;
    int scrollBarValue = 0;
    int scrollBarRange = 0;
    int timeCount = 0;
    const int timeLimit = 20;

    TestListWidgetItem *theHighlightItem = nullptr;
    TestListWidgetItem *oldHighlightItem = nullptr;
    TestListWidgetItem *theSelectedItem = nullptr;
    TestListWidgetItem *oldSelectedItem = nullptr;

    const QRect targetRect(const QPoint &position) const;
    //const QRect selectedRect(const QPoint &position) const;

private slots:
    void updateSelectedIcon();
    void doAutoScroll();

public slots:
    void getScrollBarValue(int value) {scrollBarValue = value;}
//    void getLimitedHeight(int height) {limitedHeight = height;}
    void getscrollBarRange(int min, int max) {Q_UNUSED(min) scrollBarRange = max;}

signals:
    void exceptedValue(int value);
    void Interval(int interval);
    void beginDrag();
    void endDrag();
};

class TestListWidgetItem : public QListWidgetItem
{
public:
    explicit TestListWidgetItem(QListWidget *view = nullptr);
    explicit TestListWidgetItem(const QIcon &icon, const QIcon &icon_hover, QString &text, QListWidget *view = nullptr);
    void setUpIcon(const QIcon &icon, const QIcon &icon_hover);

    QIcon Img;
    QIcon Img_hover;

};

class TestListModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit TestListModel(QListView *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

//    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
//                      int row, int column, const QModelIndex &parent) override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    QStringList mimeTypes() const override;
    Qt::DropActions supportedDropActions() const override;

};

class TestListViewItem : public QStandardItem
{
public:
    explicit TestListViewItem();
    explicit TestListViewItem(const QIcon &icon, const QIcon &icon_hover, const QString &text);

    QIcon Img;
    QIcon Img_hover;
};

class TestListView : public QListView
{
    Q_OBJECT

public:
    explicit TestListView(QWidget *parent = nullptr);

    bool isDraging() const {return IsDraging;}
    int offset() const {return 19;}
    int highlightedRow() const {return theHighlightedRow;}
    int dragRow() const {return theDragRow;}
    //int selectedRow() const {return theSelectedRow;}
    static QString myMimeType() { return QStringLiteral("TestListView/text-icon"); }

protected:
    //void mousePressEvent(QMouseEvent *event) override;
    //void mouseReleaseEvent(QMouseEvent *event) override;
    //void mouseMoveEvent(QMouseEvent *event) override;
//    void dragEnterEvent(QDragEnterEvent *event) override;
//    void dragLeaveEvent(QDragLeaveEvent *event) override;
//    void dragMoveEvent(QDragMoveEvent *event) override;
//    void dropEvent(QDropEvent *event) override;
//    void startDrag(Qt::DropActions supportedActions) override;

private:
    //QPoint startPos;

    bool IsDraging = false;
    int theHighlightedRow = -2;
    int oldHighlightedRow = -2;
    int theDragRow = -1;
    //int theSelectedRow = -1;
    int theInsertRow = -1;

};

#define POLYGON 4   //等腰三角形直角边长
#define WIDTH 1     //分隔符粗细的一半

class TestItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    TestItemDelegate(QObject *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};

#define SHADOW_WIDTH 10                 // 窗口阴影宽度;
#define TRIANGLE_WIDTH 15               // 小三角的宽度;
#define TRIANGLE_HEIGHT 10              // 小三角的高度;
#define BORDER_RADIUS 5                 // 窗口边角的弧度;

class TestArrowWidget : public QWidget
{
    Q_OBJECT

public:
    TestArrowWidget(QWidget *parent = nullptr);
    void setStartPos(int startX); // 设置小三角起始位置;
    void setTriangleInfo(int width, int height); // 设置小三角宽和高;

protected:
    void paintEvent(QPaintEvent *) override;
    //bool event(QEvent *event) override;

private:
    int m_startX; // 小三角起始位置;
    int m_triangleWidth; // 小三角的宽度;
    int m_triangleHeight; // 小三角高度;
    void initUi();
};

#endif // MYCLASS_H
