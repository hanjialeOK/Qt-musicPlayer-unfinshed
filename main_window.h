#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QDesktopWidget>
#include <QMediaPlaylist>
#include <QHeaderView>
#include <QTime>
#include <QTimer>
#include <QStackedWidget>

#include "widget_Helper/framelesswindow.h"
#include "widget_Helper/arrowwidget.h"
#include "widget_Bar/title_bar.h"
#include "widget_Bar/player_bar.h"
#include "widget_Bar/list_bar.h"
#include "widget_Bar/musicinfo_bar.h"
#include "widget_Children/widget.h"
#include "widget_Children/widget_1.h"
#include "widget_Children/widget_localmusic.h"

class CollapseButton;

class Main_Window : public CFramelessWindow
{
    Q_OBJECT

public:
    explicit Main_Window(QWidget *parent = nullptr);
    ~Main_Window() override;

private:
    TitleBar *titleBar = nullptr;
    ListBar *listBar = nullptr;
    Widget *discoverMusic = nullptr;
    PlayerBar *playerBar = nullptr;
    MusicInfoBar *musicInfoBar = nullptr;
    widget_1 *childwindow_setup = nullptr;
    LocalMusic *localMusic = nullptr;
    QWidget *mainWidget = nullptr;
    QStackedWidget *mainStackedWidget = nullptr;
    QStackedWidget *rightStackedWidget = nullptr;
    QPushButton *collapseButton = nullptr;

    qint64 m_duration;
    QMediaPlayer *MediaPlayer = nullptr;
    QMediaPlaylist *MediaPlaylist = nullptr;
    QHash<QUrl, MpegHelper::MP3_INFO> MediaPlaylistInfo;  //哈希表将歌曲地址和歌曲信息绑定
    QList<MpegHelper::MP3_INFO> playlist;  //当前列表
    MpegHelper::MP3_INFO mpegInfo; //当前播放歌曲信息
    qint64 Position = 0;
    QTimer *timer;
    ArrowWidget *tipmsg = nullptr;
    int locallistflag = -2;

    QSystemTrayIcon *systemTrayIcon = nullptr;
    QMenu *menu = nullptr;
    QAction *showMainAction = nullptr;
    QAction *exitAppAction = nullptr;
    QAction *openSetupAction = nullptr;

private:
    void initUi();
    void createActions();
    void createMenu();
    void updateDurationInfo(qint64 currentInfo);
    void on_pushButton_2_clicked();
    void ShowTipMsg();

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent * event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
//    void paintEvent(QPaintEvent *event);

private slots:
    void on_activatedsystemTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void Handle_systemTrayMenu();   //响应托盘菜单
    void on_SetupButton_clicked();  //响应标题栏中的设置按钮
    void on_CloseButton_clicked();  //响应标题栏中的关闭按钮
    void Handle_childwindow_Signals();  //负责处理子窗口关闭事件

    void on_Item_1_clicked();
    void on_Item_2_clicked();
    void on_Item_3_clicked();
    void on_Item_4_clicked();
    void on_Item_5_clicked();
    void on_Item_6_clicked();
    void on_Item_7_clicked();
    void on_Item_8_clicked();

    void metaDataChanged();
    void playlistPositionChanged(int index);
    void statusChanged(QMediaPlayer::MediaStatus status);
    void displayErrorMessage();
    void Handle_CustomSlider_Clicked(int value);
    void GetsongInfo(QUrl url, MpegHelper::MP3_INFO mp3_Info);
    void localMusicDoubleClicked(QModelIndex index);
    void handleMediaChanged(/*QList<MpegHelper::MP3_INFO> playlist, */int index/*, int flag*/);
    void CloseTipMsg();
    void updatePlaylist(int dragRow, int insertRow);

signals:
    void NewMusicInfo(QString Title, QString Author, QPixmap Image);
    void PositionJumpedMsg(MpegHelper::MP3_INFO mp3Info, int index, int flag);
    void songsCount(int count);
};

class CollapseButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CollapseButton(QWidget *parent = nullptr);
    void closeButton();

private:
    enum state { State_Normal, State_Hover };
    QTimer *timerN1 = nullptr;
    QTimer *timerN2 = nullptr;
    QPainterPath ellipse;
    state btnState;
    QPoint clickPos;
    int radius;
    int color;

protected:
    //bool event(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

//signals:
//    void pButtonClicked();
};

#endif // MAIN_WINDOW_H
