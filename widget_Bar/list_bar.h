#ifndef LIST_BAR_H
#define LIST_BAR_H

#include <QWidget>
#include <QApplication>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QMouseEvent>
#include <QDebug>
#include <QRect>
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QScrollArea>
#include <QScrollBar>
#include <QMenu>
#include <QProxyStyle>
#include <QWidgetAction>
#include <QTimer>
#include <QThread>
#include <windows.h>
#include <dwmapi.h>

#include "widget_Helper/draganddrop.h"
#include "widget_Helper/arrowwidget.h"

class ItemContentsWidget;
class CustomListWidget;
class CustomItemDelegate;
class MyListWidget;
class MyItemDelegate;
class MyScrollBar;
class MyMenu;
class CCustomStyle;
class QActionWidget;
class MyTimer;
class PPushButton;

#define POLYGON 4   //等腰三角形直角边长
#define WIDTH 1     //分隔符粗细的一半

class ListBar : public QWidget
{
    Q_OBJECT

public:
    explicit ListBar(QWidget *parent = nullptr);

private:
    QFrame *listFrame = nullptr;
    QLabel *RecommendLabel = nullptr;
    QLabel *MyMusicLabel = nullptr;
    QPushButton *CreatedDirButton = nullptr;
    PPushButton *AddDirButton = nullptr;
    CustomListWidget *RecommendList = nullptr;
    CustomListWidget *MyMusicList = nullptr;
    MyListWidget *CreatedDirList = nullptr;
    QListWidgetItem *Item_1 = nullptr;
    QListWidgetItem *Item_2 = nullptr;
    QListWidgetItem *Item_3 = nullptr;
    QListWidgetItem *Item_4 = nullptr;
    QListWidgetItem *Item_5 = nullptr;
    QListWidgetItem *Item_6 = nullptr;
    QListWidgetItem *Item_7 = nullptr;
    QListWidgetItem *Item_8 = nullptr;
    ItemContentsWidget *ItemWidget_1 = nullptr;
    ItemContentsWidget *ItemWidget_2 = nullptr;
    ItemContentsWidget *ItemWidget_3 = nullptr;
    ItemContentsWidget *ItemWidget_4 = nullptr;
    ItemContentsWidget *ItemWidget_5 = nullptr;
    ItemContentsWidget *ItemWidget_6 = nullptr;
    ItemContentsWidget *ItemWidget_7 = nullptr;
    ItemContentsWidget *ItemWidget_8 = nullptr;
    CustomItemDelegate *RecommendDelegate = nullptr;
    CustomItemDelegate *MyMusicDelegate = nullptr;
    MyItemDelegate *CreatedDirDelegate = nullptr;
    QWidget *mainWidget = nullptr;
    MyScrollBar *theShownScrollBar = nullptr;
    QScrollArea *ScrollArea = nullptr;
    AddDirWidget *addDirWidget = nullptr;

    MyMenu *Menu = nullptr;
    QWidgetAction *play = nullptr;
    QWidgetAction *playnext = nullptr;
    QWidgetAction *share = nullptr;
    QWidgetAction *rename = nullptr;
    QWidgetAction *remove = nullptr;
    QActionWidget *play_ActionWidget = nullptr;
    QActionWidget *playnext_ActionWidget = nullptr;
    QActionWidget *share_ActionWidget = nullptr;
    QActionWidget *rename_ActionWidget = nullptr;
    QActionWidget *remove_ActionWidget = nullptr;

    bool IsFolded = true;
    int theRowByMenu = -1;

private:
    void initUi();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void on_ListWidget_Clicked();
    void on_CreatedDirList_Clicked(QListWidgetItem *item);
    void on_AddDirButton_Clicked();
    void on_CreatedDirButton_Clicked();
    void showContextMenu(int row);
    void HandleMenuClicked();

public slots:
    void insert(const QString &str);

signals:
    void Item_1_Clicked();
    void Item_2_Clicked();
    void Item_3_Clicked();
    void Item_4_Clicked();
    void Item_5_Clicked();
    void Item_6_Clicked();
    void Item_7_Clicked();
    void Item_8_Clicked();
    void isItem_1Selected(bool);
    void isItem_2Selected(bool);
    void isItem_3Selected(bool);
    void isItem_4Selected(bool);
    void isItem_5Selected(bool);
    void isItem_6Selected(bool);
    void isItem_7Selected(bool);
    void isItem_8Selected(bool);
    void limitedHeight(int height);
};

//*********************ItemContentsWidget***********************ItemContentsWidget**************************ItemContentsWidget*******//

class ItemContentsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemContentsWidget(QWidget *parent = nullptr);
    void setupWidget(const QIcon &icon, const QIcon &icon_hover, const QString &str);
    void setIconSize(int size);

public:
    QPushButton *ImageButton;
    QLabel *TextLabel;
    QIcon Img;
    QIcon Img_hover;

protected:
    bool event(QEvent *event) override;

private:
    void initUi();

private:
    bool isSelected = false;

public slots:
    void isItemSelected(bool SelectionMsg);
};

//*********************CustomListWidget类***********************CustomListWidget类************************CustomListWidget类****************//

class CustomListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit CustomListWidget(QWidget *parent = nullptr);

    bool isDraging() const {return IsDraging;}
    int offset() const {return 19;}
    int highlightedRow() const {return thehighlightedRow;}
    int dragRow() const {return theDragRow;}
    static QString myMimeType() { return QStringLiteral("CustomListWidget/text_icon"); }

private:
    bool IsDraging = false;
    QRect oldhighlightedRect;
    QRect thehighlightedRect;
    QPoint startPos;

    int thehighlightedRow = -2;
    int theDragRow = -1;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    //void startDrag(Qt::DropActions supportedActions) override;

private:
    const QRect targetRect(const QPoint &position) const;

};

//*********************CustomItemDelegate***********************CustomItemDelegate**************************CustomItemDelegate*******//

class CustomItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    CustomItemDelegate(QObject *parent = nullptr);

protected:
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

};

//**************************MyListWidget***************************MyListWidget***************************MyListWidget*************************//

class MyListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit MyListWidget(QWidget *parent = nullptr);

    bool isDraging() const {return IsDraging;}
    bool isDropOutside() const {return IsDropOutside;}
    int offset() const {return 19;}
    int highlightedRow() const {return thehighlightedRow;}
    int dragRow() const {return theDragRow;}
    int selectedRow() const {return theSelectedRow;}
    static QString myMimeType() { return QStringLiteral("MyListWidget/text_icon"); }

    bool isWheeling() const {return IsWheeling;}
    int wheelRow() const {return theWheelRow;}

    int rowByMenu() const {return theRowByMenu;}
    //QRect availableRect() const {return viewport()->geometry();}
    //QColor normalColor() const {return;}

private:
    MyMenu *Menu = nullptr;
    QWidgetAction *play = nullptr;
    QWidgetAction *playnext = nullptr;
    QWidgetAction *share = nullptr;
    QWidgetAction *rename = nullptr;
    QWidgetAction *remove = nullptr;
    QActionWidget *play_ActionWidget = nullptr;
    QActionWidget *playnext_ActionWidget = nullptr;
    QActionWidget *share_ActionWidget = nullptr;
    QActionWidget *rename_ActionWidget = nullptr;
    QActionWidget *remove_ActionWidget = nullptr;
    MyTimer *autoScrollTimer = nullptr;

    bool IsDraging = false;
    bool IsDropOutside = false;
    bool IsWheeling = false;
    QPoint startPos;
    QRect oldhighlightedRect;
    QRect thehighlightedRect;
    int thehighlightedRow = -1;
    int theDragRow = -2;
    int theSelectedRow = -1;
    int theInsertRow = -1;
    int theWheelRow = -1;
    int oldWheelRow = -1;
    int theRowByMenu = -1;

    QRect theRectByMenu;
    int limitedHeight;
    const int scrollMargin = 25;
    int scrollBarValue = 0;
    int scrollBarRange = 0;
    int timeCount = 0;
    const int timeLimit = 20;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    //void startDrag(Qt::DropActions supportedActions) override;
    //void timerEvent(QTimerEvent*event) override;
    bool event(QEvent *event) override;

private:
    const QRect targetRect(const QPoint &position) const;
    const QRect selectedRect(const QPoint &position) const;

private slots:
    void doAutoScroll();
    void updateItemStyle();

public slots:
    void getScrollBarValue(int value) {scrollBarValue = value;}
    void getLimitedHeight(int height) {limitedHeight = height;}
    void getscrollBarRange(int/* min*/, int max) {scrollBarRange = max;}
    void HandleMenuClosed() {theRowByMenu = -1; update(theRectByMenu);}
    void HandleScrollBarWheeled();

signals:
    void ContextMenuClicked(int row);
    void exceptedValue(int value);
    void Interval(int interval);
    void beginDrag();
    void endDrag();
};

//*********************MyItemDelegate***********************MyItemDelegate**************************MyItemDelegate*******//

class MyItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    MyItemDelegate(QObject *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

//************************MyScrollBar***********************MyScrollBar**************************MyScrollBar************************//

class MyScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    explicit MyScrollBar(QWidget *parent = nullptr);

protected:
    //void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    bool event(QEvent *event) override;

public slots:
    void getSliderRange(int min, int max);
    void scrollToValue(int value);

signals:
    void WheelMsg(QWheelEvent *event);
};

class HiddenScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    explicit HiddenScrollBar(QWidget *parent = nullptr);

protected:
    //void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    //bool event(QEvent *event) override;

public slots:
    void wheelHelper(QWheelEvent *event);

signals:
    void Wheel();
};

//**************************MyMenu**************************MyMenu**********************************MyMenu*********************************//

class MyMenu : public QMenu
{
    Q_OBJECT
public:
    explicit MyMenu(QWidget *parent = nullptr);
    explicit MyMenu(const QString &title);

protected:
    bool event(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

signals:
    void MenuIsClosed();
};

//************************CCustomStyle*****************************CCustomStyle*************************CCustomStyle*********************//

class CCustomStyle : public QProxyStyle
{
    Q_OBJECT

public:
    explicit CCustomStyle(QStyle *parent = nullptr) : QProxyStyle(parent) {}

    int pixelMetric (PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
    {
        int m = QProxyStyle::pixelMetric(metric, option, widget);
        if (metric == QStyle::PM_SmallIconSize) {
            m = 25;
        }
        return m;
    }
};

//**************************QActionWidget************************QActionWidget**************************QActionWidget************************//

class QActionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QActionWidget(QWidget *parent = nullptr);
    void setupWidget(const QIcon &icon, const QString &str);
    void setIconSize(int size);

protected:
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent *) override;

private:
    QPushButton *ImageButton = nullptr;
    QLabel *TextLabel = nullptr;
    QIcon Img;
    bool IsHover = false;

private:
    void initUi();
};

//********************CCPushButton*****************************CCPushButton*******************CCPushButton*****************************//

class PPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PPushButton(QWidget *parent = nullptr);
    void restoreIcon(const QPoint &p) {radius = 0; if(ellipse.contains(mapFromGlobal(p))) btnState = State_Hover; update();}

protected:
    //bool event(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    enum state { State_Normal, State_Hover };
    QTimer *timer = nullptr;
    state btnState;
    QPoint clickPos;
    QPainterPath ellipse;
    int radius;

signals:
    void pButtonClicked();
};

#endif // LIST_BAR_H
