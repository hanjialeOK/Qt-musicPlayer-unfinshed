#include "main_window.h"

CollapseButton::CollapseButton(QWidget *parent) :
    QPushButton(parent),
    btnState(State_Normal),
    radius(0),
    color(100)
{
    setMouseTracking(true);

    //! setMask controls the rect of button very well !
    ellipse.addEllipse(0, 0, 60, 60);
    setMask(QRegion(-1, -1, 62, 62, QRegion::Ellipse));

    timerN1 = new QTimer(this);
    connect(timerN1, &QTimer::timeout, this, [=](){ radius++; update(); });

    timerN2 = new QTimer(this);
    connect(timerN2, &QTimer::timeout, this, [=](){ color--; update(); if(color == 80) {timerN2->stop(); color = 100;}});
}


//pressd() ---215ms--- released() ---0ms--- clicked()
void CollapseButton::mousePressEvent(QMouseEvent *event)
{
    if(ellipse.contains(event->pos())){
        QPushButton::mousePressEvent(event);
        clickPos = event->pos();
        QPoint center = QPoint(width() / 2, width() / 2);
        int interval = static_cast<int>((215.0f / ((clickPos - center).manhattanLength() + 18.0f)) + 0.5f);
        timerN1->start(interval);
    }
}

void CollapseButton::mouseReleaseEvent(QMouseEvent *event)
{
    QPushButton::mouseReleaseEvent(event);

    timerN1->stop();
    radius = 0;
    update();

    timerN2->start(10);
}

void CollapseButton::mouseMoveEvent(QMouseEvent *event)
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

void CollapseButton::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);
    btnState = State_Normal;
    unsetCursor();
    update();
}

void CollapseButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);

    if(btnState == State_Hover){
        painter.setBrush(QColor(80, 80, 80));
        painter.drawPath(ellipse);
    }

    if(radius > 0){
        painter.setBrush(QColor(100, 100, 100));

        QPainterPath circle;
        circle.addEllipse(clickPos, radius, radius);
        painter.drawPath(circle.intersected(ellipse));
    }

    if(color < 100 && color > 80){
        painter.setBrush(QColor(color, color, color));
        painter.drawPath(ellipse);
    }

    QPushButton::paintEvent(event);
}

void CollapseButton::closeButton()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(200);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    connect(animation, &QPropertyAnimation::finished, this, &CollapseButton::hide);
}

//**************************Main_Window****************************Main_Window************************Main_Window***************************//

Main_Window::Main_Window(QWidget *parent) :
    CFramelessWindow(parent),
    titleBar(new TitleBar(this)),
    listBar(new ListBar(this)),
    discoverMusic(new Widget(this)),
    playerBar(new PlayerBar(this)),
    musicInfoBar(new MusicInfoBar(this)),
    mainWidget(new QWidget(this)),
    mainStackedWidget(new QStackedWidget(this)),
    rightStackedWidget(new QStackedWidget(this))
{
    initUi();
}

Main_Window::~Main_Window()
{
    delete titleBar;
    delete listBar;
    delete discoverMusic;
    delete playerBar;
    delete musicInfoBar;
    delete childwindow_setup;

    delete systemTrayIcon;
    delete menu;
    delete showMainAction;
    delete exitAppAction;
    delete openSetupAction;
}

void Main_Window::initUi()
{
//    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
//    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    setResizeableAreaWidth(5);
    setTitleBar(titleBar);
    addIgnoreWidget(titleBar->IconLabel);
    addIgnoreWidget(titleBar->TitleLabel);
    //titleBar->IconLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    //titleBar->TitleLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    resize(1130, 750);
    setMinimumSize(1130, 750);
    setMaximumSize(1920, 1030);

    installEventFilter(titleBar);  //安装标题栏的事件过滤器
    installEventFilter(playerBar);  //安装播放条的事件过滤器
    installEventFilter(listBar);
//    installEventFilter(musicInfoBar);
    installEventFilter(this);  //安装自己的事件过滤器

//    titleBar->installEventFilter(this);
//    playerBar->installEventFilter(this);
//    listBar->installEventFilter(this);

    setWindowIcon(QIcon(":/main_Icon/logo.png"));  //设置主图标,事件过滤

    //QWidget *widget = new QWidget();
    QFrame *frame = new QFrame;

    QVBoxLayout *grandchildLayout = new QVBoxLayout();
    grandchildLayout->addWidget(listBar);
    grandchildLayout->setSpacing(0);
    grandchildLayout->addWidget(musicInfoBar);
    grandchildLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *childLayout = new QHBoxLayout();
    childLayout->addLayout(grandchildLayout);
    childLayout->setSpacing(0);
    childLayout->addWidget(rightStackedWidget);
    childLayout->setContentsMargins(0, 0, 0, 0);
    mainWidget->setLayout(childLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout(frame);
    mainLayout->addWidget(titleBar);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(mainStackedWidget);
    mainLayout->addWidget(playerBar);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    //widget->setLayout(Layout);
    setCentralWidget(frame);

    mainStackedWidget->addWidget(mainWidget);
    mainStackedWidget->setCurrentWidget(mainWidget);

    rightStackedWidget->addWidget(discoverMusic);
    rightStackedWidget->setCurrentWidget(discoverMusic);

    MediaPlayer = new QMediaPlayer(this);
    MediaPlaylist = new QMediaPlaylist();
    MediaPlaylist->setPlaybackMode(QMediaPlaylist::Loop);
    MediaPlayer->setPlaylist(MediaPlaylist);
    //connect(MediaPlaylist, &QMediaPlaylist::currentIndexChanged, this, &Main_Window::handleMediaChanged);

    timer = new QTimer();
    //playerBar->setState(MediaPlayer->state());
    connect(timer, &QTimer::timeout, this, &Main_Window::CloseTipMsg);

    connect(titleBar, &TitleBar::SetupButton_Clicked, this, &Main_Window::on_SetupButton_clicked);  //标题栏中的"设置"按钮
    connect(titleBar, &TitleBar::CloseButton_Clicked, this, &Main_Window::on_CloseButton_clicked);  //标题栏中的"关闭"按钮

    connect(listBar, &ListBar::Item_1_Clicked, this, &Main_Window::on_Item_1_clicked);
    connect(listBar, &ListBar::Item_2_Clicked, this, &Main_Window::on_Item_2_clicked);
    connect(listBar, &ListBar::Item_3_Clicked, this, &Main_Window::on_Item_3_clicked);
    connect(listBar, &ListBar::Item_4_Clicked, this, &Main_Window::on_Item_4_clicked);
    connect(listBar, &ListBar::Item_5_Clicked, this, &Main_Window::on_Item_5_clicked);
    connect(listBar, &ListBar::Item_6_Clicked, this, &Main_Window::on_Item_6_clicked);
    connect(listBar, &ListBar::Item_7_Clicked, this, &Main_Window::on_Item_7_clicked);
    connect(listBar, &ListBar::Item_8_Clicked, this, &Main_Window::on_Item_8_clicked);

    connect(MediaPlayer, QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &Main_Window::metaDataChanged);
    //connect(MediaPlaylist, &QMediaPlaylist::currentIndexChanged, this, &Main_Window::playlistPositionChanged);
    connect(MediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Main_Window::statusChanged);
    connect(MediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &Main_Window::displayErrorMessage);
//    connect(MediaPlayer, &QMediaPlayer::stateChanged, this, &Main_Window::stateChanged);

    connect(MediaPlayer, &QMediaPlayer::durationChanged, playerBar, &PlayerBar::durationChanged);
    connect(MediaPlayer, &QMediaPlayer::positionChanged, playerBar, &PlayerBar::positionChanged);
    connect(MediaPlayer, &QMediaPlayer::stateChanged, playerBar, &PlayerBar::setState);

    connect(this, &Main_Window::NewMusicInfo, musicInfoBar, &MusicInfoBar::updateMusicInfo);
    connect(this, &Main_Window::songsCount, playerBar, &PlayerBar::SetNumforSongsCountButton);

    connect(playerBar, &PlayerBar::play, MediaPlayer, &QMediaPlayer::play);
    connect(playerBar, &PlayerBar::pause, MediaPlayer, &QMediaPlayer::pause);
    connect(playerBar, &PlayerBar::next, MediaPlaylist, &QMediaPlaylist::next);
    connect(playerBar, &PlayerBar::previous, MediaPlaylist, &QMediaPlaylist::previous);
    connect(playerBar, &PlayerBar::changeVolume, MediaPlayer, &QMediaPlayer::setVolume);
    connect(playerBar, &PlayerBar::setMediaPlaybackMode, MediaPlaylist, &QMediaPlaylist::setPlaybackMode);
    //connect(playerBar->MusicListButton, &QPushButton::clicked, this, &Main_Window::on_pushButton_2_clicked);
    connect(playerBar,  //快速定位
            &PlayerBar::CustomSlider_Clicked,  //单击滑条发射自定义信号
            this,
            &Main_Window::Handle_CustomSlider_Clicked);

    move((QApplication::desktop()->width() - width())/2,
         (QApplication::desktop()->height() - height())/2);

    collapseButton = new QPushButton(this);
    collapseButton->setFixedSize(60, 60);
    collapseButton->setIconSize(QSize(30, 30));
    collapseButton->setIcon(QIcon(QStringLiteral(":/main_Icon/collapse.png")));
    collapseButton->setCursor(Qt::PointingHandCursor);
    collapseButton->setFlat(true);
    collapseButton->setStyleSheet(
                "QPushButton{background-color:rgba(230, 230, 230, 120); border:0px; border-radius:30px;}"
                "QPushButton:hover{background-color:rgba(210, 210, 210, 150);}"
                "QPushButton:pressed{background-color:rgba(200, 200, 200, 150);}"
                );
    collapseButton->setFocusPolicy(Qt::NoFocus);
    collapseButton->hide();
    connect(collapseButton, &QPushButton::clicked, this, [=](){
        if(mainStackedWidget->currentWidget() == mainWidget){
            if(musicInfoBar->isVisible()){
                listBar->hide();
                musicInfoBar->hide();
                collapseButton->setIcon(QIcon(QStringLiteral(":/main_Icon/expand.png")));
            }else{
                listBar->show();
                musicInfoBar->show();
                collapseButton->setIcon(QIcon(QStringLiteral(":/main_Icon/collapse.png")));
            }
            collapseButton->hide();
        }
    });

//    titleBar->hide();
//    listBar->hide();
//    discoverMusic->hide();
//    playerBar->hide();
//    musicInfoBar->hide();

}

//void Main_Window::paintEvent(QPaintEvent *event)
//{
//    Q_UNUSED(event);
//    QPainterPath path;
//    path.setFillRule(Qt::WindingFill);
//    path.addRect(10, 10, width() - 20, height() - 20);
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.fillPath(path, QBrush(Qt::white));
//    QColor color(92, 93, 95, 50);
//    int arr[10] = {150, 120, 80, 50, 40, 30, 20, 10, 5, 5};
//    for(int i=0; i<10; i++){
//        QPainterPath path;
//        path.setFillRule(Qt::WindingFill);
//        if(i == 5)
//            path.addRect(10-i-1, 10-i-1, width()-(10-i-1)*2, height()-(10-i-1)*2);
//        else
//            path.addRoundedRect(10-i-1, 10-i-1, width()-(10-i-1)*2, height()-(10-i-1)*2, 2, 2);
//        color.setAlpha(arr[i]);
//        painter.setPen(color);
//        painter.drawPath(path);
//    }
//}

void Main_Window::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::question(this,
                             "退出",
                             "Are you sure to quit this application?",
                             QMessageBox::Yes, QMessageBox::No)
            == QMessageBox::Yes){
        if(systemTrayIcon){
            //systemTrayIcon->hide();
            systemTrayIcon->deleteLater();
        }
        event->accept();
        qDebug()<<"应用程序退出...";
    }
    else event->ignore();
}

void Main_Window::changeEvent(QEvent *event)
{
    if(event->type() != QEvent::WindowStateChange) return;
    if(windowState() == Qt::WindowMaximized){
//        if(childwindow_setup){
//            childwindow_setup->move(mapFromGlobal(QPoint(0, 60)));
//        }
//        if(localMusic){
//            localMusic->move(mapFromGlobal(QPoint(250, 60)));
//        }
    }
    else{
//        if(childwindow_setup){
//            childwindow_setup->move(0, 60);
//        }
//        if(localMusic){
//            localMusic->move(250, 60);
//        }
    }
}

bool Main_Window::eventFilter(QObject *obj, QEvent *event)
{
    switch(event->type()){
    case QEvent::Resize:
//        if(childwindow_setup){
//            childwindow_setup->resize(titleBar->width(), discoverMusic->height());
//        }
//        if(localMusic){
//            localMusic->resize(titleBar->width() - 250, discoverMusic->height());
//        }
//        if(tipmsg){
//            tipmsg->move(width() - tipmsg->width(), height() - 60 - tipmsg->height() + 12 + 10);
//        }
        break;
    case QEvent::HoverMove:
        if(mainStackedWidget->currentWidget() == mainWidget){
            QPoint pos = mapFromGlobal(QCursor::pos());
            if(pos.y() > 60 && pos.y() < height() - 60 && pos.x() < 350 && listBar->isVisible()){
                collapseButton->move(260 + (isMaximized() ? 9 : 0), (height() - 120) / 2 - 30 + 60);
                collapseButton->raise();
                collapseButton->show();
            }else if(pos.y() > 60 && pos.y() < height() - 60 && pos.x() < 100 && !listBar->isVisible()){
                collapseButton->move(10 + (isMaximized() ? 9 : 0), (height() - 120) / 2 - 30 + 60);
                collapseButton->raise();
                collapseButton->show();
            }
            else collapseButton->hide();
        }
        break;
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}

void Main_Window::on_SetupButton_clicked()  //响应标题栏中的设置按钮
{
    if(!childwindow_setup){
        childwindow_setup = new widget_1(this);
        connect(childwindow_setup,  //"设置子窗口"关闭时发送"关闭"信号
                &widget_1::CloseInfo,
                this,
                &Main_Window::Handle_childwindow_Signals);
        installEventFilter(childwindow_setup);
        mainStackedWidget->addWidget(childwindow_setup);
    }
    mainStackedWidget->setCurrentWidget(childwindow_setup);
    qDebug()<<"打开设置...";
}

void Main_Window::on_Item_1_clicked(){}
void Main_Window::on_Item_2_clicked(){}
void Main_Window::on_Item_3_clicked(){}
void Main_Window::on_Item_4_clicked(){}

void Main_Window::on_Item_5_clicked()
{
    if(!localMusic){
        localMusic = new LocalMusic(this);
        installEventFilter(localMusic);
        //connect(localMusic, &LocalMusic::songInfo, this, &Main_Window::GetsongInfo);
        //connect(localMusic, &LocalMusic::itemDoubleClicked, this, &Main_Window::localMusicDoubleClicked);
        connect(localMusic, &LocalMusic::closeInfo, this, &Main_Window::Handle_childwindow_Signals);
        connect(localMusic, &LocalMusic::dragMsg, this, &Main_Window::updatePlaylist);
        connect(MediaPlaylist, &QMediaPlaylist::currentIndexChanged, localMusic, &LocalMusic::updatePlayingRow);
        connect(MediaPlayer, &QMediaPlayer::stateChanged, localMusic, &LocalMusic::setState);
        connect(localMusic, &LocalMusic::play, MediaPlayer, &QMediaPlayer::play);
        connect(localMusic, &LocalMusic::pause, MediaPlayer, &QMediaPlayer::pause);
        connect(localMusic, &LocalMusic::mediaChanged, this, &Main_Window::handleMediaChanged);
        connect(this, &Main_Window::PositionJumpedMsg, localMusic, &LocalMusic::positionJumped);
        rightStackedWidget->addWidget(localMusic);
    }
    rightStackedWidget->setCurrentWidget(localMusic);
    qDebug()<<"打开本地音乐...";
}

void Main_Window::on_Item_6_clicked(){}
void Main_Window::on_Item_7_clicked(){}
void Main_Window::on_Item_8_clicked(){}

void Main_Window::Handle_childwindow_Signals()
{
    //QWidget *Received_Signal = qobject_cast<QWidget *>(sender());  //获取发送信号的对象
    if(sender() == childwindow_setup){
        mainStackedWidget->setCurrentWidget(mainWidget);
        qDebug()<<"关闭设置...";
    }else if(sender() == localMusic){
        rightStackedWidget->setCurrentWidget(discoverMusic);
        qDebug()<<"关闭本地音乐...";
    }
}

//***************************播放器*****************************播放器***********************************播放器*****************************//

void Main_Window::metaDataChanged()
{
    mpegInfo = MediaPlaylistInfo.find(MediaPlaylist->currentMedia().canonicalUrl()).value();
    emit NewMusicInfo(mpegInfo.Title, mpegInfo.Artist, mpegInfo.Pictrue);
    emit PositionJumpedMsg(mpegInfo, MediaPlaylist->currentIndex(), locallistflag);
    //qDebug()<<mpegInfo.Title<<"  "<<MediaPlaylist->currentIndex()<<"  "<<locallistflag;
}

void Main_Window::playlistPositionChanged(int/* index*/)
{
}

void Main_Window::statusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
        break;
    case QMediaPlayer::LoadingMedia:
        break;
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        break;
    case QMediaPlayer::StalledMedia:
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
        displayErrorMessage();
        break;
    }
}

void Main_Window::displayErrorMessage()
{
    qDebug()<<MediaPlayer->errorString();
}

void Main_Window::Handle_CustomSlider_Clicked(int value)
{
    MediaPlayer->setPosition(value * 1000);
}

void Main_Window::on_pushButton_2_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("选择一首歌曲"), "/", "*.mp3");
    QFileInfo fileInfo = QFileInfo(path);
    QString fileName = fileInfo.baseName();
    MediaPlayer->setMedia(QUrl::fromLocalFile(path));
    MediaPlayer->play();
}

void Main_Window::GetsongInfo(QUrl url, MpegHelper::MP3_INFO mp3_Info)
{
    Q_UNUSED(url)
    Q_UNUSED(mp3_Info)
}

void Main_Window::localMusicDoubleClicked(QModelIndex index)
{
    if(index.row() == MediaPlaylist->currentIndex()){
        switch(MediaPlayer->state()){
        case QMediaPlayer::PlayingState:
            qDebug()<<"歌曲正在播放";
            return;
        default:
            MediaPlayer->play();
            return;
        }
    }
    MediaPlayer->blockSignals(true);
    MediaPlayer->setPlaylist(MediaPlaylist);  //这条语句会导致MediaPlayer播放第一首歌曲,发出metaDataChanged()
    MediaPlayer->blockSignals(false);

    MediaPlaylist->setCurrentIndex(index.row());
    MediaPlayer->play();
}

void Main_Window::updatePlaylist(int dragRow, int insertRow)
{
    if(MediaPlaylist->isEmpty()) return;

    if(dragRow != MediaPlaylist->currentIndex()) MediaPlaylist->blockSignals(true);
    if(insertRow > dragRow)
        MediaPlaylist->moveMedia(dragRow, insertRow - 1);
    else
        MediaPlaylist->moveMedia(dragRow, insertRow);
    MediaPlaylist->blockSignals(false);
}

void Main_Window::handleMediaChanged(int index)
{
    if(!MpegHelper::equal(localMusic->songs(), playlist)){
        playlist = localMusic->songs();
        MediaPlayer->pause();
        //MediaPlayer->stop();
        if(playlist.at(index).Url.url() == mpegInfo.Url.url()){
            Position = MediaPlayer->position();
            //disconnect(MediaPlayer, &QMediaPlayer::positionChanged, playerBar, &PlayerBar::positionChanged);
        }
        MediaPlayer->blockSignals(true);
        MediaPlayer->setPlaylist(new QMediaPlaylist());  //这条语句会发出metaDataChanged()
        MediaPlayer->blockSignals(false);

        MediaPlaylist->clear();
        MediaPlaylistInfo.clear();
        for(int i = 0; i < playlist.size(); i++){
            MediaPlaylist->addMedia(playlist.at(i).Url);
            MediaPlaylistInfo.insert(playlist.at(i).Url, playlist.at(i));
        }
        MediaPlayer->blockSignals(true);
        MediaPlayer->setPlaylist(MediaPlaylist);  //这条语句会导致MediaPlayer播放第一首歌曲,发出metaDataChanged()
        MediaPlayer->blockSignals(false);

        emit songsCount(playlist.size());
        qDebug()<<"歌单已更改...";
    }
    else if(index == MediaPlaylist->currentIndex()){  //相同歌单,相同歌曲
        switch(MediaPlayer->state()){
        case QMediaPlayer::PlayingState:
            qDebug()<<"歌曲正在播放";
            break;
        default:
            MediaPlayer->play();
            break;
        }
        ShowTipMsg();
        return;
    }

    MediaPlaylist->setCurrentIndex(index);
    MediaPlayer->play();
    if(Position){
        MediaPlayer->setPosition(Position);
        //connect(MediaPlayer, &QMediaPlayer::positionChanged, playerBar, &PlayerBar::positionChanged);
        Position = 0;
    }
    ShowTipMsg();
}

void Main_Window::ShowTipMsg()
{
    if(!tipmsg) tipmsg = new ArrowWidget(this);
    tipmsg->textLabel->setText("已开始播放");
    tipmsg->setStartPos(85);
    tipmsg->setTriangleInfo(20, 12);
    tipmsg->setFixedSize(150, 70);
    tipmsg->move(width() - tipmsg->width(), height() - 60 - tipmsg->height() + 12 + 10);
    tipmsg->show();
    tipmsg->raise();
    timer->start(3000);
}

void Main_Window::CloseTipMsg()
{
    if(tipmsg->isVisible()) tipmsg->hide();
}

//**************************创建托盘**************************创建托盘**********************************创建托盘*****************************//
void Main_Window::on_CloseButton_clicked()  //响应标题栏中的关闭按钮
{
    hide();
    if(!systemTrayIcon){
        systemTrayIcon = new QSystemTrayIcon(this);
        QIcon icon = QIcon(":/tray_Icon/tray_logo.png");
        systemTrayIcon->setIcon(icon);
        systemTrayIcon->setToolTip("application");
        connect(systemTrayIcon,
                &QSystemTrayIcon::activated,
                this,
                &Main_Window::on_activatedsystemTrayIcon);
        createActions();
        createMenu();
    }
    systemTrayIcon->show();
    qDebug()<<"最小化至托盘...";
}

void Main_Window::on_activatedsystemTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
        case QSystemTrayIcon::Trigger:
            activateWindow();  //窗口显示的时候出现在最顶层
            if(!isVisible()) setVisible(true);
            QApplication::alert(this);
            qDebug()<<"单击还原主窗口...";
            //menu->exec(QCursor::pos());  //让菜单显示在鼠标的位置上
            //if(systemTrayIcon->geometry().contains(QCursor::pos())) qDebug()<<"显示菜单...";
            //menu->move(cursor().pos());
            //menu->show();
            break;
        case QSystemTrayIcon::DoubleClick:
            activateWindow();  //窗口显示的时候出现在最顶层
            if(!isVisible()) setVisible(true);
            //QApplication::alert(this);  //任务栏图标闪烁
            qDebug()<<"双击还原主窗口...";
            break;
        default:
            qDebug()<<"显示托盘菜单...";
            break;
    }
}

void Main_Window::createActions()
{
    showMainAction = new QAction("显示主面板", this);
//    showMainAction->setIcon(QIcon(":/tray_Icon/tray_home.png"));
    connect(showMainAction,
            &QAction::triggered,
            this,
            &Main_Window::Handle_systemTrayMenu);
    openSetupAction = new QAction("设置", this);
    openSetupAction->setIcon(QIcon(":/tray_Icon/tray_setup.png"));
    connect(openSetupAction,
            &QAction::triggered,
            this,
            &Main_Window::Handle_systemTrayMenu);
    exitAppAction = new QAction("退出", this);
    exitAppAction->setIcon(QIcon(":/tray_Icon/tray_quit.png"));
    connect(exitAppAction,
            &QAction::triggered,
            this,
            &Main_Window::Handle_systemTrayMenu);
}

void Main_Window::createMenu()
{
    menu = new QMenu(this);
    menu->addAction(showMainAction);
    menu->addSeparator();
    menu->addAction(openSetupAction);
    menu->addSeparator();
    menu->addAction(exitAppAction);
    menu->setFont(QFont("宋体", 10, QFont::Bold));
    systemTrayIcon->setContextMenu(menu);
}

void Main_Window::Handle_systemTrayMenu()
{
    QAction *triggered_Action = qobject_cast<QAction *>(sender());
    if(triggered_Action == showMainAction){
        activateWindow();  //窗口显示的时候出现在最顶层
        show();
        menu->hide();
        QApplication::alert(this);  //任务栏图标闪烁
        qDebug()<<"还原主窗口...";
    }
    else if(triggered_Action == openSetupAction){
        activateWindow();
        show();
        on_SetupButton_clicked();
        menu->hide();
        QApplication::alert(childwindow_setup);
    }
    else if(triggered_Action == exitAppAction){
        //systemTrayIcon->hide();
        systemTrayIcon->deleteLater();
        qDebug()<<"应用程序退出...";
        QApplication::quit();
    //    exit(0);
    }
}
