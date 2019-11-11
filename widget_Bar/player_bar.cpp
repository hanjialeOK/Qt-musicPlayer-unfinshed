#include "widget_Bar/player_bar.h"

CustomSlider::CustomSlider(QWidget *parent) :
    QSlider(parent)
{
}

CustomSlider::~CustomSlider()
{
}

void CustomSlider::mousePressEvent(QMouseEvent *event)
{
    //注意应先调用父类的鼠标点击处理事件，这样可以不影响拖动的情况
    QSlider::mousePressEvent(event);
    double pos = static_cast<double>(event->pos().x() - 6) / static_cast<double>(width() - 15); //6和15是修正系数
    if(!isSliderDown()){  //防止双击拖动handle时激发此信号
        setValue(static_cast<int>(pos * (maximum() - minimum()) + minimum()));
        emit CustomSliderClicked();
    }
}

//***************************PlayerBar类**************************PlayerBar类************************PlayerBar***************************//

PlayerBar::PlayerBar(QWidget *parent) :
    QWidget(parent)
{
    initUi();
}

PlayerBar::~PlayerBar()
{
    delete frame;
    delete LastOneButton;
    delete BeginOrPauseButton;
    delete NextOneButton;
    delete SoundVolumeButton;
    delete PlayerPatternButton;
    delete PlaylistButton;
    delete SongsCountButton;
    delete ProgressBarSlider;
    delete SoundVolumeSlider;
    delete TimeProgressLabel;
    delete TimeDurationLabel;
}

void PlayerBar::initUi()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    setFixedHeight(60);
    resize(1130, 60);

    frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFixedWidth(1130 + 18);
    frame->setStyleSheet("QFrame{border-top:1px solid rgb(200, 200, 200);}");
    frame->move(0, 0);

    LastOneButton = new QPushButton(this);
    BeginOrPauseButton = new QPushButton(this);
    NextOneButton = new QPushButton(this);
    TimeProgressLabel = new QLabel(this);
    ProgressBarSlider = new CustomSlider(this);
    TimeDurationLabel = new QLabel(this);
    SoundVolumeButton = new QPushButton(this);
    SoundVolumeSlider = new CustomSlider(this);
    PlayerPatternButton = new QPushButton(this);
    PlaylistButton = new QPushButton(this);
    SongsCountButton = new QPushButton(this);

    LastOneButton->setObjectName("LastOneButton");
    BeginOrPauseButton->setObjectName("BeginOrPauseButton");
    NextOneButton->setObjectName("NextOneButton");
    TimeProgressLabel->setObjectName("TimeProgressLabel");
    ProgressBarSlider->setObjectName("ProgressBarSlider");
    TimeDurationLabel->setObjectName("TimeDurationLabel");
    SoundVolumeButton->setObjectName("SoundVolumeButton");
    SoundVolumeSlider->setObjectName("SoundVolumeSlider");
    PlayerPatternButton->setObjectName("PlayerPatternButton");
    PlaylistButton->setObjectName("PlaylistButton");
    SongsCountButton->setObjectName("SongsCountButton");

    LastOneButton->setFocusPolicy(Qt::NoFocus);
    BeginOrPauseButton->setFocusPolicy(Qt::NoFocus);
    NextOneButton->setFocusPolicy(Qt::NoFocus);
    TimeProgressLabel->setFocusPolicy(Qt::NoFocus);
    ProgressBarSlider->setFocusPolicy(Qt::NoFocus);
    TimeDurationLabel->setFocusPolicy(Qt::NoFocus);
    SoundVolumeButton->setFocusPolicy(Qt::NoFocus);
    SoundVolumeSlider->setFocusPolicy(Qt::NoFocus);
    PlayerPatternButton->setFocusPolicy(Qt::NoFocus);
    PlaylistButton->setFocusPolicy(Qt::NoFocus);
    SongsCountButton->setFocusPolicy(Qt::NoFocus);

    LastOneButton->setCursor(Qt::PointingHandCursor);
    BeginOrPauseButton->setCursor(Qt::PointingHandCursor);
    NextOneButton->setCursor(Qt::PointingHandCursor);
    ProgressBarSlider->setCursor(Qt::PointingHandCursor);
    SoundVolumeButton->setCursor(Qt::PointingHandCursor);
    SoundVolumeSlider->setCursor(Qt::PointingHandCursor);
    PlayerPatternButton->setCursor(Qt::PointingHandCursor);
    PlaylistButton->setCursor(Qt::PointingHandCursor);
    SongsCountButton->setCursor(Qt::PointingHandCursor);

    LastOneButton->setToolTip("上一首");
    BeginOrPauseButton->setToolTip("播放");
    NextOneButton->setToolTip("下一首");
    SoundVolumeButton->setToolTip("静音");
    PlayerPatternButton->setToolTip("循环播放");
    PlaylistButton->setToolTip("打开播放列表");
    SongsCountButton->setToolTip("打开播放列表");

    LastOneButton->setFixedSize(40, 40);  //上一首
    LastOneButton->setIconSize(QSize(37, 37));
    LastOneButton->setIcon(QIcon(":/playerBar_Icon/lastone.png"));
    LastOneButton->setFlat(true);
    LastOneButton->setStyleSheet("QWidget{background-color:rgb(240, 240, 240);}");

    BeginOrPauseButton->setFixedSize(50, 50);  //暂停/播放
    BeginOrPauseButton->setIconSize(QSize(43, 43));
    BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/pause.png"));
    BeginOrPauseButton->setFlat(true);
    BeginOrPauseButton->setStyleSheet("QWidget{background-color:rgb(240, 240, 240);}");

    NextOneButton->setFixedSize(40, 40);  //下一首
    NextOneButton->setIconSize(QSize(37, 37));
    NextOneButton->setIcon(QIcon(":/playerBar_Icon/nextone.png"));
    NextOneButton->setFlat(true);
    NextOneButton->setStyleSheet("QWidget{background-color:rgb(240, 240, 240);}");

    TimeProgressLabel->setFixedSize(41, 15);  //显示进度
    TimeProgressLabel->setText("00:00");
    TimeProgressLabel->setFont(QFont("Calibri", 10, QFont::Medium));

    ProgressBarSlider->setOrientation(Qt::Horizontal);  //进度条
    ProgressBarSlider->setFixedSize(520, 31);  //
    ProgressBarSlider->setRange(0, 240);
    QString ProgressBarSlider_Style =
            "QSlider::groove:horizontal{"
            "border: 1px solid rgba(0, 0, 0, 0);"
//            "border-radius: 10px;"
//            "height: 19px;"
            "}"
            "QSlider::sub-page:horizontal{"
            "background: rgb(232, 60, 60);"
            "border: 1px solid rgba(0, 0, 0, 0);"
            "border-radius:2px;"
            "margin-top:13px;"  //31-26=5无论怎样,保证结果是5
            "margin-bottom:13px;"
            "margin-left:6px;"
            "}"
            "QSlider::add-page:horizontal{"
            "background: rgb(194, 194, 194);"
            "border: 1px solid rgba(0, 0, 0, 0);"
            "border-radius: 2px;"
            "margin-top:13px;"
            "margin-bottom:13px;"
            "margin-right:6px;"
            "}"
            "QSlider::handle:horizontal{"
            //"background: rgb(255, 153, 102);"
            //"border: 3px solid rgba(183, 183, 183, 200);"
            "background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,"
            "stop:0.1 rgba(200, 0, 0, 255), stop:0.4 rgba(255, 255, 255, 255), stop:0.78 rgba(255, 255, 255, 255), stop:1 rgba(183, 183, 183, 255));"
            "width: 17px;"
            "height:17px;"
            "border-radius: 8px;"
            "margin-top:6px;"  //31-12=19,无论怎样,保证结果是19,这一结果和17有关
            "margin-bottom:6px;"
            "}"
            "QSlider::handle:horizontal:hover {"
            //"background: rgb(255,128,6);"
            //"border: 2px solid rgba(102,102,102,102);"
            //"border-image:url(:/playerBar_Icon/handle_hover.png);"
            "background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,"
            //"stop:0.1 rgba(200, 0, 0, 255), stop:0.3236 rgba(255, 255, 255, 255), stop:0.631 rgba(255, 255, 255, 255), stop:0.809 rgba(183, 183, 183, 200), stop:1 rgba(200, 200, 200, 100));"
            "stop:0.1 rgba(200, 0, 0, 255), stop:0.4 rgba(255, 255, 255, 255), stop:0.78 rgba(255, 255, 255, 255), stop:1 rgba(183, 183, 183, 255));"
//            "width: 21px;"
//            "height:21px;"
//            "border-radius: 10px;"
//            "margin-top:4px;"  //31-12=19,无论怎样,保证结果是19,这一结果和17有关
//            "margin-bottom:4px;"
            "}"
            "QSlider::sub-page:horizontal:disabled {"
            "background: #bbb;"
            "border-color: #999;"
            "}"
            "QSlider::add-page:horizontal:disabled {"
            "background: #eee;"
            "border-color: #999;"
            "}"
            "QSlider::handle:horizontal:disabled {"
            "background: #eee;"
            "border: 1px solid #aaa;"
            "border-radius: 4px;"
            "}";
    ProgressBarSlider->setStyleSheet(ProgressBarSlider_Style);

    TimeDurationLabel->setFixedSize(41, 15);  //总时长
    TimeDurationLabel->setText("04:00");
    TimeDurationLabel->setFont(QFont("Calibri", 10, QFont::Medium));

    SoundVolumeButton->setFixedSize(23, 23);  //静音
    SoundVolumeButton->setIconSize(QSize(23, 23));
    SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_2.png"));
    SoundVolumeButton->setFlat(true);

    SoundVolumeSlider->setOrientation(Qt::Horizontal);  //音量条
    SoundVolumeSlider->setFixedSize(130, 20);
    SoundVolumeSlider->setRange(0, 100);
    SoundVolumeSlider->setValue(50);
    QString SoundVolumeSlider_Style =
            "QSlider::groove:horizontal{"
            "border: 1px solid rgba(0, 0, 0, 0);"
//            "border-radius: 10px;"
//            "height: 5px;"
            "}"
            "QSlider::sub-page:horizontal{"
            "background: rgb(232, 60, 60);"
            "border: 1px solid rgba(0, 0, 0, 0);"
            "border-radius: 1px;"
            "margin-top:8px;"
            "margin-bottom:8px;"
            "margin-left:4px;"
            "}"
            "QSlider::add-page:horizontal{"
            "background: rgb(194, 194, 194);"
            "border: 1px solid rgba(0, 0, 0, 0);"
            "border-radius: 1px;"
            "margin-top:8px;"
            "margin-bottom:8px;"
            "margin-right:4px;"
            "}"
            "QSlider::handle:horizontal{"
            //"background: rgb(255, 153, 102);"
            //"border: 1px solid rgb(183, 183, 183);"
            "background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,"
            "stop:0.1 rgba(200, 0, 0, 255), stop:0.4 rgba(255, 255, 255, 255), stop:0.7 rgba(255, 255, 255, 255), stop:1 rgba(183, 183, 183, 255));"
            "width:14px;"
            "height:14px;"
            "border-radius:7px;"
            "margin-top:2px;"
            "margin-bottom:2px;"
            "}"
            "QSlider::handle:horizontal:hover {"
            //"background: rgb(255,128,6);"
            //"border: 1px solid rgba(102,102,102,102);"
            "background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,"
            "stop:0.1 rgba(200, 0, 0, 255), stop:0.4 rgba(255, 255, 255, 255), stop:0.7 rgba(255, 255, 255, 255), stop:1 rgba(183, 183, 183, 255));"
            "border-radius: 7px;"
            "}"
            "QSlider::sub-page:horizontal:disabled {"
            "background: #bbb;"
            "border-color: #999;"
            "}"
            "QSlider::add-page:horizontal:disabled {"
            "background: #eee;"
            "border-color: #999;"
            "}"
            "QSlider::handle:horizontal:disabled {"
            "background: #eee;"
            "border: 1px solid #aaa;"
            "border-radius: 4px;"
            "}";
    SoundVolumeSlider->setStyleSheet(SoundVolumeSlider_Style);

    PlayerPatternButton->setFixedSize(30, 30);  //播放模式
    PlayerPatternButton->setIconSize(QSize(24, 24));
    PlayerPatternButton->setIcon(QIcon(":/playerBar_Icon/circle.png"));
    PlayerPatternButton->setFlat(true);

    PlaylistButton->setFixedSize(27, 27);  //播放列表
    PlaylistButton->setIconSize(QSize(27, 27));
    PlaylistButton->setIcon(QIcon(":/playerBar_Icon/playlist.png"));
    PlaylistButton->setStyleSheet("QPushButton{border:0px solid rgb(50, 50, 50);}");
    PlaylistButton->raise();

    SongsCountButton->setFixedSize(37, 21);
    SongsCountButton->setText("400");
    SongsCountButton->setFont(QFont("微软雅黑", 8, QFont::Medium));
    SongsCountButton->setStyleSheet(
                "QPushButton{border:0px; border-top-right-radius:8px; border-bottom-right-radius:8px;}"
                "QPushButton{background-color:rgb(220, 220, 220); color:rgba(30, 30, 30, 230);}"
                );

    LastOneButton->setStyleSheet("QPushButton{border:0px solid rgb(100, 100, 100);}");
    BeginOrPauseButton->setStyleSheet("QPushButton{border:0px solid rgb(100, 100, 100);}");
    NextOneButton->setStyleSheet("QPushButton{border:0px solid rgb(100, 100, 100);}");
    SoundVolumeButton->setStyleSheet("QPushButton{border:0px solid rgb(100, 100, 100);}");
    PlayerPatternButton->setStyleSheet("QPushButton{border:0px solid rgb(100, 100, 100);}");
    //PlaylistButton->setStyleSheet("QPushButton{border:0px solid rgb(100, 100, 100);;}");

    LastOneButton->installEventFilter(this);
    BeginOrPauseButton->installEventFilter(this);
    NextOneButton->installEventFilter(this);
    SoundVolumeButton->installEventFilter(this);
    PlayerPatternButton->installEventFilter(this);
    PlaylistButton->installEventFilter(this);
    SongsCountButton->installEventFilter(this);
    ProgressBarSlider->installEventFilter(this);

    QHBoxLayout *Layout = new QHBoxLayout(this);
    Layout->addSpacing(20);
    Layout->addWidget(LastOneButton);  //上一首
    Layout->addSpacing(11);
    Layout->addWidget(BeginOrPauseButton);  //暂停/播放
    Layout->addSpacing(11);
    Layout->addWidget(NextOneButton);  //下一首
    Layout->addSpacing(20);
    Layout->addWidget(TimeProgressLabel);  //显示进度
    Layout->addSpacing(1);
    Layout->addWidget(ProgressBarSlider);  //进度条
    Layout->addSpacing(0);
    Layout->addWidget(TimeDurationLabel);  //总时长
    Layout->addSpacing(20);
    Layout->addWidget(SoundVolumeButton);  //静音
    Layout->addSpacing(-7);
    Layout->addWidget(SoundVolumeSlider);  //音量滑条
    Layout->addSpacing(12);
    Layout->addWidget(PlayerPatternButton);  //播放模式
    Layout->addStretch();
    Layout->addWidget(PlaylistButton);  //播放列表
    Layout->addSpacing(-15);
    Layout->addWidget(SongsCountButton);  //歌曲总数
    Layout->addSpacing(15);
    Layout->setContentsMargins(5, 0, 5, 0);  //设置左上右下边距
    setLayout(Layout);

    QPalette pal(palette());  //设置播放条颜色
    pal.setColor(QPalette::Background, QColor(246, 246, 248));
    setAutoFillBackground(true);
    setPalette(pal);

    connect(LastOneButton, &QPushButton::clicked, this, &PlayerBar::Handle_Button_CLicked);
    connect(BeginOrPauseButton, &QPushButton::clicked, this, &PlayerBar::Handle_Button_CLicked);
    connect(NextOneButton, &QPushButton::clicked, this, &PlayerBar::Handle_Button_CLicked);
    connect(SoundVolumeButton, &QPushButton::clicked, this, &PlayerBar::Handle_Button_CLicked);
    connect(SoundVolumeSlider, &QSlider::valueChanged, this, &PlayerBar::changeVolume);
    connect(PlayerPatternButton, &QPushButton::clicked, this, &PlayerBar::Handle_Button_CLicked);
//    connect(MusicListButton, &QPushButton::clicked, this, &PlayerBar::Handle_Button_CLicked);

    connect(ProgressBarSlider,  //读取进度条
            &QSlider::valueChanged,
            this,
            &PlayerBar::updateDurationInfo);
    connect(ProgressBarSlider,  //快速定位
            &CustomSlider::CustomSliderClicked,  //单击滑条发射自定义信号
            this,
            &PlayerBar::send_CustomSlider_Clicked);
    connect(ProgressBarSlider,  //快速定位
            &CustomSlider::sliderReleased,  //双击拖动滑条后释放发射此信号
            this,
            &PlayerBar::send_CustomSlider_Clicked);
    connect(SoundVolumeSlider,  //由音量条改变音量图标
            &QSlider::valueChanged,
            this,
            &PlayerBar::ShowVioceVolumeState);

//    connect(SongList,
//            &QTableView::doubleClicked,  //双击列表播放歌曲
//            this,
//            &PlayerBar::myPlayerSlot);
//    connect(SoundVolumeSlider,  //由音量条改变播放器音量
//            static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged),
//            MediaPlayer,
//            &QMediaPlayer::setVolume);
//    connect(timer,
//            &QTimer::timeout,
//            this,
//            &PlayerBar::setPosition);

}
//**************************播放器*****************************播放器***********************************播放器*****************************//
QMediaPlayer::State PlayerBar::state() const
{
    return playerState;
}

void PlayerBar::setState(QMediaPlayer::State state)
{
    if (state != playerState) {
        playerState = state;

        switch (state) {
        case QMediaPlayer::StoppedState:
            BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/pause.png"));
            break;
        case QMediaPlayer::PlayingState:
            BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/begin.png"));
            break;
        case QMediaPlayer::PausedState:
            BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/pause.png"));
            break;
        }
    }
}

void PlayerBar::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;
    ProgressBarSlider->setMaximum(static_cast<int>(m_duration));

    QString Str_totalTime;
    QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
                    m_duration % 60, (m_duration * 1000) % 1000);
    QString format = "mm:ss";
    if (m_duration > 3600)
        format = "hh:mm:ss";
    Str_totalTime = totalTime.toString(format);
    if(m_duration) TimeDurationLabel->setText(Str_totalTime);
}

void PlayerBar::positionChanged(qint64 progress)
{
    if(!ProgressBarSlider->isSliderDown())
        ProgressBarSlider->setValue(static_cast<int>(progress) / 1000);
}

void PlayerBar::updateDurationInfo(int value)
{
    QString Str_currentTime;
    QTime currentTime((value / 3600) % 60, (value / 60) % 60,
        value % 60, (value * 1000) % 1000);
    QString format = "mm:ss";
    if (m_duration > 3600)
        format = "hh:mm:ss";
    Str_currentTime = currentTime.toString(format);
    TimeProgressLabel->setText(Str_currentTime);
}

void PlayerBar::SetNumforSongsCountButton(int count)
{
    SongsCountButton->setText(QString::number(count));
}

void PlayerBar::send_CustomSlider_Clicked()
{
    emit CustomSlider_Clicked(ProgressBarSlider->value());
}

//void PlayerBar::ShowTimeProgress()
//{
//    minute = ProgressBarSlider->value() / 60;
//    second = ProgressBarSlider->value() % 60;
////    QString str_minute= QString::number(minute);
////    QString str_second= QString::number(second);
//    TimeProgressLabel->setText(QString("%1").arg(minute, 2, 10, QLatin1Char('0'))
//                               + ":"
//                               + tr("%1").arg(second, 2, 10, QLatin1Char('0')));
//    TimeDurationLabel->setText(QString("%1").arg((ProgressBarSlider->maximum() / 60), 2, 10, QLatin1Char('0'))
//                               + ":"
//                               + tr("%1").arg((ProgressBarSlider->maximum() % 60), 2, 10, QLatin1Char('0')));
//}

void PlayerBar::ShowVioceVolumeState()
{
    if(SoundVolumeSlider->value() >= 10) VioceVolume = SoundVolumeSlider->value();
    if(SoundVolumeSlider->value() > 80){
        SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_3.png"));
        SoundVolumeButton->setToolTip("静音");
    }
    else if(SoundVolumeSlider->value() > 20){
        SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_2.png"));
        SoundVolumeButton->setToolTip("静音");
    }
    else if(SoundVolumeSlider->value() > 0){
        SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_1.png"));
        SoundVolumeButton->setToolTip("静音");
    }
    else{
        SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_mute.png"));
        SoundVolumeButton->setToolTip("恢复音量");
    }
}

bool PlayerBar::eventFilter(QObject *obj, QEvent *event)
{
    switch(event->type()){
    case QEvent::Resize:{
        frame->setFixedWidth(width());
        ProgressBarSlider->setFixedWidth(width() - 1130 - 18 + 520);
        break;
    }
    case QEvent::MouseButtonPress:
        break;
    case QEvent::HoverEnter:
        if(obj == LastOneButton){
            LastOneButton->setIcon(QIcon(":/playerBar_Icon/lastone_hover.png"));
            return true;
        }
        else if(obj == BeginOrPauseButton){
            switch (state()) {
            case QMediaPlayer::PlayingState:
                BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/begin_hover.png"));
                return true;
            default:
                BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/pause_hover.png"));
                return true;
            }
        }
        else if(obj == NextOneButton){
            NextOneButton->setIcon(QIcon(":/playerBar_Icon/nextone_hover.png"));
            return true;
        }
        else if(obj == SoundVolumeButton){
            if(SoundVolumeSlider->value() > 70){
                SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_3_hover.png"));
                return true;
            }
            else if(SoundVolumeSlider->value() > 30){
                SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_2_hover.png"));
                return true;
            }
            else if(SoundVolumeSlider->value() > 0){
                SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_1_hover.png"));
                return true;
            }
            else{
                SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_mute_hover.png"));
                return true;
            }
        }
        else if(obj == PlayerPatternButton){
            if(PlayerPattern_Helper == Circle){
                PlayerPatternButton->setIcon(QIcon(":/playerBar_Icon/circle_hover.png"));
                return true;
            }
            else if(PlayerPattern_Helper == Repeat){
                PlayerPatternButton->setIcon(QIcon(":/playerBar_Icon/repeat_hover.png"));
                return true;
            }
            else if(PlayerPattern_Helper == Random_){
                PlayerPatternButton->setIcon(QIcon(":/playerBar_Icon/random_hover.png"));
                return true;
            }
            else if(PlayerPattern_Helper == Sequence_){
                PlayerPatternButton->setIcon(QIcon(":/playerBar_Icon/sequence_hover.png"));
                return true;
            }
        }
        else if(obj == PlaylistButton || obj == SongsCountButton){
            PlaylistButton->setIcon(QIcon(":/playerBar_Icon/playlist_hover.png"));
            SongsCountButton->setStyleSheet(
                        "QPushButton{background-color:rgb(215, 215, 215); color:rgba(0, 0, 0, 255);}"
                        "QPushButton{border:0px; border-top-right-radius:8px; border-bottom-right-radius:8px;}"
                        );
            return true;
        }
        break;
    case QEvent::HoverLeave:
        if(obj == LastOneButton){
            LastOneButton->setIcon(QIcon(":/playerBar_Icon/lastone.png"));
            return true;
        }
        else if(obj == BeginOrPauseButton){
            switch (state()) {
            case QMediaPlayer::PlayingState:
                BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/begin.png"));
                return true;
            default:
                BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/pause.png"));
                return true;
            }
        }
        else if(obj == NextOneButton){
            NextOneButton->setIcon(QIcon(":/playerBar_Icon/nextone.png"));
            return true;
        }
        else if(obj == SoundVolumeButton){
            if(SoundVolumeSlider->value() > 70){
                SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_3.png"));
                return true;
            }
            else if(SoundVolumeSlider->value() > 30){
                SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_2.png"));
                return true;
            }
            else if(SoundVolumeSlider->value() > 0){
                SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_1.png"));
                return true;
            }
            else{
                SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_mute.png"));
                return true;
            }
        }
        else if(obj == PlayerPatternButton){
            if(PlayerPattern_Helper == Circle){
                PlayerPatternButton->setIcon(QIcon(":/playerBar_Icon/circle.png"));
                return true;
            }
            else if(PlayerPattern_Helper == Repeat){
                PlayerPatternButton->setIcon(QIcon(":/playerBar_Icon/repeat.png"));
                return true;
            }
            else if(PlayerPattern_Helper == Random_){
                PlayerPatternButton->setIcon(QIcon(":/playerBar_Icon/random.png"));
                return true;
            }
            else if(PlayerPattern_Helper == Sequence_){
                PlayerPatternButton->setIcon(QIcon(":/playerBar_Icon/sequence.png"));
                return true;
            }
        }
        else if(obj == PlaylistButton || obj == SongsCountButton){
            PlaylistButton->setIcon(QIcon(":/playerBar_Icon/playlist.png"));
            SongsCountButton->setStyleSheet(
                        "QPushButton{background-color:rgb(220, 220, 220); color:rgba(30, 30, 30, 230);}"
                        "QPushButton{border:0px; border-top-right-radius:8px; border-bottom-right-radius:8px;}"
                        );
            return true;
        }
        break;
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}

void PlayerBar::Handle_Button_CLicked()
{
    QPushButton *Button = qobject_cast<QPushButton *>(sender());
    if(Button == LastOneButton){  //上一首
//        if(MediaPlaylist->currentIndex()!=0){  //这里注意的是，和我们正常数数一样，mediaCount是从1开始计数的
//            //myPlayer->setMedia(playList->media(playList->previousIndex()));
//            MediaPlaylist->setCurrentIndex(MediaPlaylist->previousIndex());
//            MediaPlayer->play();
//        }
//        else{
//            MediaPlaylist->setCurrentIndex(MediaPlaylist->mediaCount()-1);
//            //myPlayer->setMedia(playList->media(playList->mediaCount()-1));
//            MediaPlayer->play();
//        }
        emit previous();
        emit play();
    }
    else if(Button == BeginOrPauseButton){  //播放/暂停
        switch (state()) {
        case QMediaPlayer::StoppedState:
            emit play();
            BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/begin_hover.png"));
            break;
        case QMediaPlayer::PlayingState:
            emit pause();
            BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/pause_hover.png"));
            break;
        case QMediaPlayer::PausedState:
            emit play();
            BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/begin_hover.png"));
            break;
        }
    }
    else if(Button == NextOneButton){
//        if(MediaPlaylist->mediaCount()-MediaPlaylist->currentIndex()!=1){
//            //myPlayer->setMedia(playList->media(playList->nextIndex()));
//            MediaPlaylist->setCurrentIndex(MediaPlaylist->nextIndex());
//            MediaPlayer->play();
//        }
//        else{
//            MediaPlaylist->setCurrentIndex(0);
//           // myPlayer->setMedia(playList->media(0));
//            MediaPlayer->play();
//        }
        emit next();
        emit play();
    }
    else if(Button == SoundVolumeButton){
        if(SoundVolumeSlider->value()){
            SoundVolumeSlider->setValue(0);
            SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_mute_hover.png"));
            QToolTip::showText(QCursor::pos(), "恢复音量");
        }
        else{
            SoundVolumeSlider->setValue(VioceVolume);
            if(SoundVolumeSlider->value() > 70)
                SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_3_hover.png"));
            else if(SoundVolumeSlider->value() > 30)
                SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_2_hover.png"));
            else
                SoundVolumeButton->setIcon(QIcon(":/playerBar_Icon/volume_1_hover.png"));
            QToolTip::showText(QCursor::pos(), "静音");
        }
    }
    else if(Button == PlayerPatternButton){
        PlayerPattern_Helper++;
        PlayerPattern_Helper = PlayerPattern_Helper % 4;
        if(PlayerPattern_Helper == Circle){  //0
            emit setMediaPlaybackMode(QMediaPlaylist::Loop);
            PlayerPatternButton->setIcon(QIcon(":/playerBar_Icon/circle_hover.png"));
            PlayerPatternButton->setToolTip("列表循环");
            QToolTip::showText(QCursor::pos(), PlayerPatternButton->toolTip());
        }
        else if(PlayerPattern_Helper == Repeat){  //1
            emit setMediaPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
            PlayerPatternButton->setIcon(QIcon(":/playerBar_Icon/repeat_hover.png"));
            PlayerPatternButton->setToolTip("单曲循环");
            QToolTip::showText(QCursor::pos(), PlayerPatternButton->toolTip());
        }
        else if(PlayerPattern_Helper == Random_){  //2
            emit setMediaPlaybackMode(QMediaPlaylist::Random);
            PlayerPatternButton->setIcon(QIcon(":/playerBar_Icon/random_hover.png"));
            PlayerPatternButton->setToolTip("随机播放");
            QToolTip::showText(QCursor::pos(), PlayerPatternButton->toolTip());
        }
        else if(PlayerPattern_Helper == Sequence_){  //3
            emit setMediaPlaybackMode(QMediaPlaylist::Sequential);
            PlayerPatternButton->setIcon(QIcon(":/playerBar_Icon/sequence_hover.png"));
            PlayerPatternButton->setToolTip("顺序播放");
            QToolTip::showText(QCursor::pos(), PlayerPatternButton->toolTip());
        }
    }
    else if(Button == PlaylistButton){
        if(!is_MusicListButton_pressed){
            is_MusicListButton_pressed = true;
        }
        else{
            is_MusicListButton_pressed = false;
        }
    }
}

//*******************************播放器*****************************播放器************************************播放器****************************//

//void PlayerBar::on_importBtn_clicked()
//{
//    path = QFileDialog::getOpenFileNames(this,"选择歌曲","/","(*.mp3)");
//    int i = 0;
//    QString addSongs;
//    QListIterator<QString> mylist(path);
//    while(mylist.hasNext())
//    {
//        addSongs = mylist.next();
//        QFileInfo fileInfo;
//        fileInfo = QFileInfo(addSongs);
//        QString fileName = fileInfo.fileName();
//        SongListModel->setItem(i, 0, new QStandardItem(fileName));
//        MediaPlaylist->addMedia(QUrl::fromLocalFile(addSongs));
//        i++;
//    }
//    MediaPlaylist->setPlaybackMode(QMediaPlaylist::Loop);
//    MediaPlayer->setPlaylist(MediaPlaylist);
//}

//void PlayerBar::on_pushButton_2_clicked()
//{
//    QString path = QFileDialog::getOpenFileName(this, tr("选择一首歌曲"), "/", "*.mp3");
//    QFileInfo fileInfo = QFileInfo(path);
//    QString fileName = fileInfo.baseName();
//    MediaPlayer->setMedia(QUrl::fromLocalFile(path));
//    BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/begin.png"));
//    is_BeginOrPauseButton_pressed = true;
//    timer->start(500);
//    MediaPlayer->play();
//}

//void PlayerBar::myPlayerSlot(QModelIndex index)
//{
//    timer->start(500);
//    MediaPlaylist->setCurrentIndex(index.row());
////    myPlayer->setMedia(playList->currentMedia());这句话不能加，加了反而错误
//    BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/begin.png"));
//    is_BeginOrPauseButton_pressed = true;
//    MediaPlayer->play();
//}

//void PlayerBar::setPosition()
//{
//    QString strTitle = MediaPlayer->metaData("Title").toString();
//    QString strAuthor = MediaPlayer->metaData("Author").toString();
//    QString strSize= MediaPlayer->metaData("Size").toString();
//    emit NewMusicInfo(strTitle, strAuthor);
//    qDebug()<<"title: " + strTitle + "; " + "Author:" +  strAuthor + "; " + "size: " + strSize;
//    ProgressBarSlider->setMaximum(static_cast<int>(MediaPlayer->duration()) / 1000);
//    ProgressBarSlider->setSliderPosition(static_cast<int>(MediaPlayer->position()) / 1000);
//    if(MediaPlayer->position() == MediaPlayer->duration()){
//        timer->stop();
//        BeginOrPauseButton->setIcon(QIcon(":/playerBar_Icon/pause.png"));
//        is_BeginOrPauseButton_pressed = false;
//    }
//}
