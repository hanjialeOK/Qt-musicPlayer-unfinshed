#include "widget_Bar/title_bar.h"

//*************************CCPushButton**************************CCPushButton*****************************CCPushButton***********************//

CCPushButton::CCPushButton(QWidget *parent) :
    QPushButton(parent),
    btnState(State_Normal),
    radius(0),
    color(100)
{
    setMouseTracking(true);

    // setMask controls the rect of button very well !
    ellipse.addEllipse(0, 0, 36, 36);
    setMask(QRegion(0 - 1, 0 - 1, 36 + 2, 36 + 2, QRegion::Ellipse));

    timerN1 = new QTimer(this);
    connect(timerN1, &QTimer::timeout, this, [=](){ radius++; update(); });

    timerN2 = new QTimer(this);
    connect(timerN2, &QTimer::timeout, this, [=](){
        if(cnt > 8){
            if(radius > 0) radius = 0;
            color--;
            update();
            if(color == 80) {
                timerN2->stop();
                color = 100;
            }
        }
        else cnt++;
    });
}


//pressd() ---215ms--- released() ---0ms--- clicked()
void CCPushButton::mousePressEvent(QMouseEvent *event)
{
    if(ellipse.contains(event->pos())){
        QPushButton::mousePressEvent(event);
        clickPos = event->pos();
        QPoint center = QPoint(width() / 2, width() / 2);
        int interval = static_cast<int>((215.0f / ((clickPos - center).manhattanLength() + 18.0f)) + 0.5f);
        timerN1->start(interval);
    }
}

void CCPushButton::mouseReleaseEvent(QMouseEvent *event)
{
    QPushButton::mouseReleaseEvent(event);

    timerN1->stop();
    //radius = 0;
    update();

    cnt = 0;
    timerN2->start(15);
}

void CCPushButton::mouseMoveEvent(QMouseEvent *event)
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

void CCPushButton::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);
    btnState = State_Normal;
    unsetCursor();
    update();
}

void CCPushButton::paintEvent(QPaintEvent *event)
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

//*************************TitleBar**************************TitleBar******************************TitleBar*****************************//

TitleBar::TitleBar(QWidget *parent) :
    QWidget(parent)
{
    initUi();
}

TitleBar::~TitleBar()
{
    delete frame;
    delete IconLabel;
    delete TitleLabel;
    delete VlineLabel;
    delete ChangeSkinButton;
    delete MessageButton;
    delete MinimizeButton;
    delete MaximizeButton;
}

void TitleBar::initUi()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    setFixedHeight(60);  //标题栏高度
    resize(1130, 60);
    setFocusPolicy(Qt::StrongFocus);

    frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setGeometry(0, 59, 1920, 1);
    frame->setStyleSheet("QFrame{border-top:1px solid rgb(200, 200, 200);}");

    IconLabel = new QLabel(this);
    IconLabel->setFixedSize(25, 25);  //调整宽度和高度  //图标
    IconLabel->setScaledContents(true);
    IconLabel->setCursor(Qt::PointingHandCursor);
    IconLabel->setFocusPolicy(Qt::NoFocus);

    TitleLabel = new QLabel(this);
    TitleLabel->setFixedSize(135, 35);  //标题
    TitleLabel->setScaledContents(true);
    TitleLabel->setPixmap(QPixmap(":/main_Icon/title_Icon.png"));
//    TitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    TitleLabel->setCursor(Qt::PointingHandCursor);
    TitleLabel->setFocusPolicy(Qt::NoFocus);

    VlineLabel = new QLabel(this);
    VlineLabel->setFixedSize(20, 25);  //用于分隔符
    VlineLabel->setScaledContents(true);
    VlineLabel->setText("|");
    VlineLabel->setFont(QFont("Microsoft YaHei", 15, QFont::DemiBold));
    VlineLabel->setStyleSheet("QLabel{color:red;}");
    VlineLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    ChangeSkinButton = new CCPushButton(this);
    ChangeSkinButton->setFixedSize(36, 36);  //换肤
    ChangeSkinButton->setIconSize(QSize(22, 22));
    ChangeSkinButton->setIcon(QIcon(":/tittleBar_Icon/changeskin_hover.png"));
    ChangeSkinButton->setFlat(true);
    ChangeSkinButton->setStyleSheet(
                "QPushButton{background-color:transparent; border:0px; border-radius:18px;}"
                //"QPushButton:hover{background-color:rgb(80, 80, 80);}"
                );
    //ChangeSkinButton->setCursor(Qt::PointingHandCursor);
    ChangeSkinButton->setFocusPolicy(Qt::NoFocus);
    //ChangeSkinButton->setToolTip("ChangeSkin");

    MessageButton = new CCPushButton(this);
    MessageButton->setFixedSize(36, 36);  //消息
    MessageButton->setIconSize(QSize(19, 19));
    MessageButton->setIcon(QIcon(":/tittleBar_Icon/message_hover.png"));
    MessageButton->setFlat(true);
    MessageButton->setStyleSheet(
                "QPushButton{background-color:transparent; border:0px; border-radius:18px;}"
                //"QPushButton:hover{background-color:rgb(80, 80, 80);}"
                );
    //MessageButton->setCursor(Qt::PointingHandCursor);
    MessageButton->setFocusPolicy(Qt::NoFocus);
    //MessageButton->setToolTip("Message");

    SetupButton = new CCPushButton(this);
    SetupButton->setFixedSize(36, 36);  //设置
    SetupButton->setIconSize(QSize(18, 18));
    SetupButton->setIcon(QIcon(":/tittleBar_Icon/setup_hover.png"));
    SetupButton->setFlat(true);
    SetupButton->setStyleSheet(
                "QPushButton{background-color:transparent; border:0px; border-radius:18px;}"
                //"QPushButton:hover{background-color:rgb(80, 80, 80);}"
                );
    //SetupButton->setCursor(Qt::PointingHandCursor);
    SetupButton->setFocusPolicy(Qt::NoFocus);
    //SetupButton->setToolTip("Setup");

    MinimizeButton = new QPushButton(this);
    MinimizeButton->setFixedSize(55, 40);  //最小化
    MinimizeButton->setIconSize(QSize(21, 21));
    MinimizeButton->setIcon(QIcon(":/tittleBar_Icon/minimize_hover.png"));
    MinimizeButton->setFlat(true);
    MinimizeButton->setStyleSheet(
                "QPushButton{background-color:transparent; border:0px solid rgb(0, 0, 0);}"
                "QPushButton:hover{background-color:rgb(70, 70, 70);}"
                "QPushButton:pressed{background-color:rgb(100, 100, 100);}"
                );
    //MinimizeButton->setCursor(Qt::PointingHandCursor);
    MinimizeButton->setFocusPolicy(Qt::NoFocus);
    MinimizeButton->setToolTip("Minimize");

    MaximizeButton = new QPushButton(this);
    MaximizeButton->setFixedSize(55, 40);  //最大化
    MaximizeButton->setIconSize(QSize(18, 18));
    MaximizeButton->setIcon(QIcon(":/tittleBar_Icon/maximize_hover.png"));
    MaximizeButton->setFlat(true);
    MaximizeButton->setStyleSheet(
                "QPushButton{background-color:transparent; border:0px solid rgb(0, 0, 0);}"
                "QPushButton:hover{background-color:rgb(70, 70, 70);}"
                "QPushButton:pressed{background-color:rgb(100, 100, 100);}"
                );
    //MaximizeButton->setCursor(Qt::PointingHandCursor);
    MaximizeButton->setFocusPolicy(Qt::NoFocus);
    MaximizeButton->setToolTip("Maximize");

    CloseButton = new QPushButton(this);
    CloseButton->setFixedSize(55, 40);  //关闭
    CloseButton->setIconSize(QSize(22, 22));
    CloseButton->setIcon(QIcon(":/tittleBar_Icon/close_hover.png"));
    CloseButton->setFlat(true);
    CloseButton->setStyleSheet(
                "QPushButton{background-color:transparent; border:0px solid rgb(0, 0, 0);}"
                "QPushButton:hover{background-color:rgb(198, 47, 47);}"
                "QPushButton:pressed{background-color:rgb(160, 20, 20);}"
                );
    //CloseButton->setCursor(Qt::PointingHandCursor);
    CloseButton->setFocusPolicy(Qt::NoFocus);
    CloseButton->setToolTip("Close");

    connect(SetupButton, &QPushButton::clicked, this, &TitleBar::SetupButton_Clicked);
    connect(MinimizeButton, &QPushButton::clicked, this, &TitleBar::on_clicked);
    connect(MaximizeButton, &QPushButton::clicked, this, &TitleBar::on_clicked);
    connect(CloseButton, &QPushButton::clicked, this, &TitleBar::CloseButton_Clicked);

    TitleLabel->installEventFilter(this);
    //ChangeSkinButton->installEventFilter(this);
    //MessageButton->installEventFilter(this);
    //SetupButton->installEventFilter(this);
    //MinimizeButton->installEventFilter(this);
    //MaximizeButton->installEventFilter(this);
    //CloseButton->installEventFilter(this);

    QHBoxLayout *Layout = new QHBoxLayout(this);
    Layout->addSpacing(15);
    Layout->addWidget(IconLabel);
    Layout->addSpacing(0);
    Layout->addWidget(TitleLabel);
    Layout->addStretch();
    Layout->addSpacing(5);
    Layout->addWidget(ChangeSkinButton);
    Layout->addSpacing(10);
    Layout->addWidget(MessageButton);
    Layout->addSpacing(10);
    Layout->addWidget(SetupButton);
    Layout->addSpacing(15);
    Layout->addWidget(VlineLabel);
    Layout->addWidget(MinimizeButton, 0, Qt::AlignTop);
    Layout->addWidget(MaximizeButton, 0, Qt::AlignTop);
    Layout->addWidget(CloseButton, 0, Qt::AlignTop);
    Layout->setSpacing(0);
    Layout->setContentsMargins(0, 0, 0, 0);  //设置左上右下边距
    setLayout(Layout);

    QPalette pal(palette());  //设置标题栏颜色
    //pal.setColor(QPalette::Background, QColor(198, 47, 47));
    pal.setColor(QPalette::Background, QColor(20, 47, 47, 255));
    setAutoFillBackground(true);
    setPalette(pal);
}

void TitleBar::on_clicked()
{
    QPushButton *Button = qobject_cast<QPushButton *>(sender());
    if(window()->isTopLevel()){
        if(Button == MinimizeButton){
            window()->showMinimized();
        }
        else if(Button == MaximizeButton){
            if(window()->isMaximized()){
                window()->showNormal();
                MaximizeButton->setIconSize(QSize(18, 18));
                MaximizeButton->setIcon(QIcon(":/tittleBar_Icon/maximize_hover.png"));
            }
            else{
                window()->showMaximized();
                MaximizeButton->setIconSize(QSize(15, 15));
                MaximizeButton->setIcon(QIcon(":/tittleBar_Icon/update_maximize_hover.png"));
            }
        }
    }
}

void TitleBar::focusInEvent(QFocusEvent *)
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(20, 47, 47, 255));
    setAutoFillBackground(true);
    setPalette(pal);
}

void TitleBar::focusOutEvent(QFocusEvent *)
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(20, 47, 47, 100));
    setAutoFillBackground(true);
    setPalette(pal);
}

bool TitleBar::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *Widget = qobject_cast<QWidget *>(obj);
    switch(event->type()){
//        case QEvent::WindowTitleChange:
//            if(Widget){
//                 TitleLabel->setText(Widget->windowTitle())
//                 return true;
//            }
//            break;
    case QEvent::WindowIconChange:
        if(Widget){
            QIcon icon = windowIcon();
            IconLabel->setPixmap(icon.pixmap(IconLabel->size()));
            return true;
        }
        break;
    case QEvent::Resize:
        frame->setFixedWidth(width());
        break;
    case QEvent::WindowStateChange:
        updateMaximize();
        break;
    case QEvent::HoverEnter:
        if(obj == TitleLabel){
            TitleLabel->setCursor(Qt::PointingHandCursor);
        }
        if(obj == ChangeSkinButton){
            ChangeSkinButton->setIcon(QIcon(":/tittleBar_Icon/changeskin_hover.png"));
            return true;
        }
        else if(obj == MessageButton){
            MessageButton->setIcon(QIcon(":/tittleBar_Icon/message_hover.png"));
            return true;
        }
        else if(obj == SetupButton){
            SetupButton->setIcon(QIcon(":/tittleBar_Icon/setup_hover.png"));
            return true;
        }
        else if(obj == MinimizeButton){
            MinimizeButton->setIcon(QIcon(":/tittleBar_Icon/minimize_hover.png"));
            return true;
        }
        else if(obj == MaximizeButton)
            if(window()->isMaximized()){
                MaximizeButton->setIcon(QIcon(":/tittleBar_Icon/update_maximize_hover.png"));
                return true;
            }
            else{
                MaximizeButton->setIcon(QIcon(":/tittleBar_Icon/maximize_hover.png"));
                return true;
            }
        else if(obj == CloseButton){
            CloseButton->setIcon(QIcon(":/tittleBar_Icon/close_hover.png"));
            return true;
        }
        break;
    case QEvent::HoverLeave:
        if(obj == TitleLabel){
            TitleLabel->unsetCursor();
        }
        if(obj == ChangeSkinButton){
            ChangeSkinButton->setIcon(QIcon(":/tittleBar_Icon/changeskin.png"));
            return true;
        }
        else if(obj == MessageButton){
            MessageButton->setIcon(QIcon(":/tittleBar_Icon/message.png"));
            return true;
        }
        else if(obj == SetupButton){
            SetupButton->setIcon(QIcon(":/tittleBar_Icon/setup.png"));
            return true;
        }
        else if(obj == MinimizeButton){
            MinimizeButton->setIcon(QIcon(":/tittleBar_Icon/minimize.png"));
            return true;
        }
        else if(obj == MaximizeButton)
            if(window()->isMaximized()){
                MaximizeButton->setIcon(QIcon(":/tittleBar_Icon/update_maximize.png"));
                return true;
            }
            else{
                MaximizeButton->setIcon(QIcon(":/tittleBar_Icon/maximize.png"));
                return true;
            }
        else if(obj == CloseButton){
            CloseButton->setIcon(QIcon(":/tittleBar_Icon/close.png"));
            return true;
        }
        break;
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}

void TitleBar::updateMaximize()
{
    if(window()->windowState() == Qt::WindowMaximized){
        MaximizeButton->setIconSize(QSize(15, 15));
        MaximizeButton->setIcon(QIcon(":/tittleBar_Icon/update_maximize_hover.png"));
        MaximizeButton->setToolTip("Restore");
        //MaximizeButton->setProperty("maximizeProperty", "restore");
    }
    else{
        MaximizeButton->setIconSize(QSize(18, 18));
        MaximizeButton->setIcon(QIcon(":/tittleBar_Icon/maximize_hover.png"));
        MaximizeButton->setToolTip("Maximize");
        //MaximizeButton->setProperty("maximizeProperty", "maximize");
    }
}
