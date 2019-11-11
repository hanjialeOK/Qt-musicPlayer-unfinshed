#ifndef WIDGET_LOCALMUSIC_H
#define WIDGET_LOCALMUSIC_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QTreeView>
#include <QSplitter>
#include <QTabWidget>
#include <QDebug>
#include <QFileDialog>
#include <QTime>
#include <QMouseEvent>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QScrollBar>
#include <QMediaPlayer>
#include <QStyleFactory>
#include <QScrollArea>
#include <QLineEdit>
#include <QMimeData>
#include <QDrag>
#include <QApplication>
#include <QThread>
#include <QButtonGroup>
#include <QTextCodec>

#include <taglib.h>
#include <tag.h>
#include <fileref.h>
#include <id3v2tag.h>
#include <mpegfile.h>
#include <audioproperties.h>
#include <attachedpictureframe.h>

#include "widget_Helper/draganddrop.h"
#include "widget_Helper/mpeghelper.h"

class MyTableView;
class CustomTableView;
class CustomScrollBar;
class TreeView;

class LocalMusic : public QWidget
{
    Q_OBJECT

public:
    explicit LocalMusic(QWidget *parent = nullptr);
    //~LocalMusic();
    QList<MpegHelper::MP3_INFO> songs() const {return AllSongs;}

private:
    QLineEdit *SearchForMusic;
    QLineEdit *SearchForDir;
    QPushButton *playallButton;
    QPushButton *addtoplayButton;
    QPushButton *playallButton_forDir;
    QPushButton *addtoplayButton_forDir;
    QPushButton *matchButton;

    QPushButton *musicTabButton = nullptr;
    QPushButton *authorTabButton = nullptr;
    QPushButton *ablumTabButton = nullptr;
    QPushButton *dirTabButton = nullptr;

    MyTableView *localMusicView;
    MyTableView *MusicViewFromDir;
    QStandardItemModel *songlistModel;
    QStandardItemModel *songlistModel_1;
    QStandardItemModel *TreeView_model;
    QPushButton *LocalMusicButton;
    QPushButton *AddDirectoryButton;
    QPushButton *SongNumberLabel;
    TreeView *DirectoryTree;
    QTabWidget *tabWidget;
    QScrollArea *ScrollArea;
    CustomScrollBar *theShownScrollBar;
    QWidget *mainWidget;

    QHash<QString, QList<MpegHelper::MP3_INFO>> SongsSortedFromDir;
    QList<MpegHelper::MP3_INFO> AllSongs;
    QList<QString> m_whiteList;
    QMediaPlayer::State playerState = QMediaPlayer::StoppedState;
    MpegHelper::MP3_INFO CurrentMp3Info;
//    int thePlayingRow = -1;
//    int oldPlayingRow = -1;
    int playingRow_1 = -1;
    int selectedDir = -1;
    int locallistflag = -1;
    int DirForSelectedRow = -1;
    int SelectedRow = -1;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override {emit closeInfo(); event->accept();}

private:
    void initUi();
    QMediaPlayer::State state() const {return playerState;}
    void updateDataForDir(int index);
    int updateIndexForDir(MpegHelper::MP3_INFO mp3Info);
    int updateIndex(MpegHelper::MP3_INFO mp3Info);
    void updatePlayingRow_1(int index);

private slots:
    void on_AddDirectoryButton_Clicked();
    void on_indexIcon_Clicked();
    void on_tabBarClicked(int index);
    void on_treeViewClicked(QModelIndex index);
    void on_TabView_DoubleClicked(QModelIndex index);
    void selectedRowChanged(QModelIndex index);
    void updateTabButtonIcon(int, bool);

public slots:
    void updatePlayingRow(int index);
    void setState(QMediaPlayer::State state);
    void updateMpegInfoList(int dragRow, int insertRow);
    void positionJumped(MpegHelper::MP3_INFO mp3Info, int index, int flag);

signals:
    void play();
    void pause();
    void songInfo(QUrl url, MpegHelper::MP3_INFO mp3Info);
    void mediaChanged(int index);
    void limitedHeight(int height);
    void dragMsg(int dragRow, int insertRow);
    void targetPosY(int y);
    void closeInfo();
};

//***********************MyTableView*********************MyTableView***********************MyTableView******************//

class MyTableView : public QTableView
{
    Q_OBJECT
public:
    MyTableView(QWidget *parent = nullptr);

    QModelIndex hoverIndex() const {return model()->index(mHoverRow, mHoverColumn);}
    bool isTableViewEntered() const {return IsTableViewEntered;}
    bool isHeaderOrBarEntered() const {return IsHeaderOrBarEntered;}
    bool isDraging() const {return IsDraging;}
    int dragRow() const {return theDragRow;}
    int highlightedRow() const {return theHighlightedRow;}
    static QString myMimeType() { return QStringLiteral("MyTableView/num_title_author_album_duation_size"); }
    QPoint curPos() const {return curPosition;}
    void setDoubleClickEnabled(bool enabled) {doubleClickEnabled = enabled;}
    int playingRow() const {return thePlayingRow;}
    int playedRow() const {return oldPlayingRow;}
    QMediaPlayer::State state() const {return playerState;}
    void setState(QMediaPlayer::State state) {playerState = state;}
    void setPlayingRow(int row) {oldPlayingRow = thePlayingRow; thePlayingRow = row; emit targetRow(row);}
    int rowHeight() const {return verticalHeader()->defaultSectionSize();}

private:
    MyTimer *scrollBarTimer = nullptr;

    int IsTableViewEntered = false;
    int IsHeaderOrBarEntered = false;

    bool flag = false;
    bool IsDraging = false;

    int mHoverRow = -1, mHoverColumn = -1;
    int theDragRow = -1;
    int theInsertRow = -1;
    int theHighlightedRow = -2;
    int oldHighlightedRow = -1;

    QRect theRectByMenu;
    QPoint startPos;
    int limitedHeight;
    int scrollBarValue = 0;
    int scrollBarRange = 0;
    int timeCount = 0;
    const int scrollMargin = 25;
    const int timeLimit = 20;

    QPoint curPosition;
    int doubleClickEnabled = true;
    int thePlayingRow = -1;
    int oldPlayingRow = -1;
    QMediaPlayer::State playerState = QMediaPlayer::StoppedState;

private:
    int offset() const {return verticalHeader()->defaultSectionSize() / 2 - 1;}
    bool isDoubleClickEnabled() {return doubleClickEnabled;}

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    //void startDrag(Qt::DropActions supportedActions) override;
    void leaveEvent(QEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

public slots:
    void HandleSliderReleased();
    void HandleScrollBarWheeled();
    void HandleHeaderViewReleased();
    void getScrollBarValue(int value) {scrollBarValue = value;}
    void getLimitedHeight(int height) {limitedHeight = height;}
    void getscrollBarRange(int min, int max) {Q_UNUSED(min) scrollBarRange = max;}

private slots:
    void doAutoScroll();

signals:
    void exceptedValue(int value);
    void Interval(int interval);
    void beginDrag();
    void endDrag();
    void targetRow(int row);
    void dragMsg(int dragRow, int insertRow);
};

//************************StyledItemDelegate************************StyledItemDelegate**********************StyledItemDelegate************//
#define POLYGON 4   //等腰三角形直角边长
#define WIDTH 1     //分隔符粗细的一半

class ViewItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ViewItemDelegate(QObject *parent = nullptr);
    enum ButtonState
    {
        Normal,
        Hover,
        Selected
    };
private:
    QFont font;
    int m_Height;
    QScopedPointer<QPushButton> m_pButton;
    ButtonState state;
    QRect buttonRect;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;
};

//***********************CustomScrollBar*********************CustomScrollBar***********************CustomScrollBar******************//

class CustomScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    explicit CustomScrollBar(QWidget *parent = nullptr);

protected:
    //void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *) override;
    bool event(QEvent *event) override;

public slots:
    void getSliderRange(int min, int max);
    void getTargetY(int y) {targetY = static_cast<int>(static_cast<float>(y) / (maximum() + pageStep()) * height()); update();}

private:
    int targetY = -1;

signals:
    void ScrollBarReleased();
    void ScrollBarWheeled();
};

//***********************CustomHeaderView*********************CustomHeaderView***********************CustomHeaderView******************//

class CustomHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    explicit CustomHeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);

private:
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void HeaderViewReleased();

};

//*************************TreeView***********************TreeView***********************TreeView************************//

class TreeView : public QTreeView
{
    Q_OBJECT
public:
    TreeView(QWidget *parent = nullptr);

private:
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // WIDGET_LOCALMUSIC_H
