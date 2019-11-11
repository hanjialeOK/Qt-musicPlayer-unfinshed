#include "widget_Helper/arrowwidget.h"

ArrowWidget::ArrowWidget(QWidget *parent)
    : QWidget(parent)
    , m_startX(50)
    , m_triangleWidth(TRIANGLE_WIDTH)
    , m_triangleHeight(TRIANGLE_HEIGHT)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setFixedSize(150, 50);

    auto shadowEffect = new QGraphicsDropShadowEffect(this); //设置阴影边框;
    shadowEffect->setOffset(0, 0);
    shadowEffect->setColor(Qt::gray);
    shadowEffect->setBlurRadius(SHADOW_WIDTH);
    this->setGraphicsEffect(shadowEffect);

    textLabel = new QLabel(this);
    textLabel->setFont(QFont("微软雅黑", 9, QFont::Medium));
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setFocusPolicy(Qt::NoFocus);
    textLabel->setStyleSheet("QLabel{/*background-color:rgba(255, 255, 255, 255);*/ color:rgba(100, 100, 100, 230);}");

    QHBoxLayout* hMainLayout = new QHBoxLayout(this);
    hMainLayout->addWidget(textLabel);
    hMainLayout->setSpacing(0);
    hMainLayout->setContentsMargins(SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH + TRIANGLE_HEIGHT);
}

void ArrowWidget::setCenterWidget(QWidget* widget)
{
    QHBoxLayout* hMainLayout = new QHBoxLayout(this);
    hMainLayout->addWidget(widget);
    hMainLayout->setSpacing(0);
    hMainLayout->setContentsMargins(SHADOW_WIDTH, SHADOW_WIDTH + TRIANGLE_HEIGHT, SHADOW_WIDTH, SHADOW_WIDTH);
}

void ArrowWidget::setStartPos(int startX)  //设置小三角显示的起始位置;
{
    m_startX = startX;
}

void ArrowWidget::setTriangleInfo(int width, int height)
{
    m_triangleWidth = width;
    m_triangleHeight = height;
}

void ArrowWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255, 250));

    QPolygon trianglePolygon_Up;  //上三角区域;
    trianglePolygon_Up << QPoint(m_startX, m_triangleHeight + SHADOW_WIDTH);
    trianglePolygon_Up << QPoint(m_startX + m_triangleWidth / 2, SHADOW_WIDTH);
    trianglePolygon_Up << QPoint(m_startX + m_triangleWidth, m_triangleHeight + SHADOW_WIDTH);

    QPolygon trianglePolygon_Down;  //下三角区域;
    trianglePolygon_Down << QPoint(m_startX, height() - m_triangleHeight - SHADOW_WIDTH);
    trianglePolygon_Down << QPoint(m_startX + m_triangleWidth / 2, height() - SHADOW_WIDTH);
    trianglePolygon_Down << QPoint(m_startX + m_triangleWidth, height() - m_triangleHeight - SHADOW_WIDTH);

    QPainterPath drawPath;
    drawPath.addRoundedRect(QRect(SHADOW_WIDTH, SHADOW_WIDTH,
                                  width() - SHADOW_WIDTH * 2, height() - SHADOW_WIDTH * 2 - m_triangleHeight),
                                  BORDER_RADIUS, BORDER_RADIUS);  //Rect

    drawPath.addPolygon(trianglePolygon_Down);  // Triangle;
    painter.drawPath(drawPath);
}

//****************************AddDirWidget**********************************AddDirWidget***************************************************//

AddDirWidget::AddDirWidget(QWidget *parent)
    : QWidget(parent)
    , m_triangleWidth(TRIANGLE_WIDTH_1)
    , m_triangleHeight(TRIANGLE_HEIGHT_1)
    , m_width(300)
    , m_height(230)
{
    initUi();
}

void AddDirWidget::initUi()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);

    //setFixedSize(300, 230);
    setFixedSize(1920, 1920);
    //resize(1920, 1920);

    timer = new QTimer(this);
    timer->start(300);
    timer->setSingleShot(true);

    m_startX = m_height / 2 - TRIANGLE_WIDTH_1 / 2;
    X_offset = (width() - m_width) / 2;
    Y_offset = (height() - m_height) / 2;

    CreateNewDirLabel = new QLabel(this);
    CreateNewDirLabel->setFixedSize(150, 35);
    CreateNewDirLabel->setText("新建歌单");
    CreateNewDirLabel->setFont(QFont("微软雅黑", 13, QFont::Medium));
    CreateNewDirLabel->setAlignment(Qt::AlignCenter);
    CreateNewDirLabel->setStyleSheet("QLabel{color:rgba(244, 66, 150, 255); border:0px solid rgb(0, 0, 0);}");
    CreateNewDirLabel->setFocusPolicy(Qt::NoFocus);

    DirName = new QLineEdit(this);
    DirName->setFixedSize(220, 35);
    DirName->setPlaceholderText(QStringLiteral("输入新歌单标题"));
    DirName->setTextMargins(5, 0, 0, 0);
    DirName->setFont(QFont("微软雅黑", 9, QFont::Medium));
    DirName->setStyleSheet(
                "QLineEdit{background-color:transparent; color:rgba(50, 50, 50, 150);}"
                "QLineEdit{border:1px solid rgb(200, 200, 200); border-radius:4px;}"
                );
    //DirName->setFocusPolicy(Qt::NoFocus);
    DirName->setFocus();
    connect(DirName, &QLineEdit::textChanged, this, &AddDirWidget::changeState);

    ConfirmButton = new QPushButton(this);
    ConfirmButton->setFixedSize(70, 30);
    ConfirmButton->setFlat(true);
    ConfirmButton->setText("创建");
    ConfirmButton->setFont(QFont("宋体", 10, QFont::Normal));
    ConfirmButton->setStyleSheet("QPushButton{border-radius:4px; background-color:rgb(12, 114, 194); color:rgb(255, 255, 255); border:1px solid rgb(200, 200, 200);}"
                                 "QPushButton:hover{background-color:rgb(17, 103, 168); color:rgb(255, 255, 255);}"
                                 "QPushButton:disabled{background-color:rgb(150, 192, 225); color:rgb(220, 220, 220);}"
                                 "QPushButton:pressed{padding-left:3px; padding-top:3px;}"
                                 );
    ConfirmButton->setCursor(Qt::PointingHandCursor);
    ConfirmButton->setFocusPolicy(Qt::NoFocus);
    ConfirmButton->setDisabled(true);
    connect(ConfirmButton, &QPushButton::clicked, this, &AddDirWidget::on_ConfirmButton_Clicked);

    CancelButton = new QPushButton(this);
    CancelButton->setFixedSize(70, 30);
    CancelButton->setFlat(true);
    CancelButton->setText("取消");
    CancelButton->setFont(QFont("宋体", 10, QFont::Normal));
    CancelButton->setStyleSheet("QPushButton{border-radius:4px; border:1px solid rgb(200, 200, 200);}"
                                "QPushButton:hover{background-color:rgb(220, 220, 220);}"
                                "QPushButton:pressed{padding-left:3px; padding-top:3px;}"
                                );
    CancelButton->setCursor(Qt::PointingHandCursor);
    CancelButton->setFocusPolicy(Qt::NoFocus);
    connect(CancelButton, &QPushButton::clicked, this, &AddDirWidget::on_CancelButton_Clicked);

    auto shadowEffect = new QGraphicsDropShadowEffect(this); //设置阴影边框;
    shadowEffect->setOffset(0, 0);
    shadowEffect->setColor(Qt::gray);
    shadowEffect->setBlurRadius(SHADOW_WIDTH_1);
    this->setGraphicsEffect(shadowEffect);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget(ConfirmButton);
    hlayout->setSpacing(20);
    hlayout->addWidget(CancelButton);
    hlayout->addStretch();
    hlayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addSpacing(15);
    vlayout->addWidget(CreateNewDirLabel, 0, Qt::AlignHCenter);
    vlayout->addSpacing(15);
    vlayout->addWidget(DirName, 0, Qt::AlignHCenter);
    vlayout->addSpacing(40);
    vlayout->addLayout(hlayout);
    vlayout->addStretch();
    vlayout->setContentsMargins(m_triangleHeight + SHADOW_WIDTH_1 + X_offset,
                                SHADOW_WIDTH_1 + Y_offset,
                                SHADOW_WIDTH_1 + X_offset,
                                SHADOW_WIDTH_1);
    setLayout(vlayout);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(100);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
}

void AddDirWidget::setStartPos(int startX)  //设置小三角显示的起始位置;
{
    m_startX = startX;
}

void AddDirWidget::setTriangleInfo(int width, int height)
{
    m_triangleWidth = width;
    m_triangleHeight = height;
}

void AddDirWidget::changeState(const QString &str)
 {
    if(str.isEmpty())
        ConfirmButton->setDisabled(true);
    else
        ConfirmButton->setDisabled(false);
}

void AddDirWidget::on_ConfirmButton_Clicked()
{
    emit restoreButtonIcon();
    emit Confirmed(DirName->text());
    DirName->clear();
    //deleteLater();
    closeWidget();
}

void AddDirWidget::on_CancelButton_Clicked()
{
    emit restoreButtonIcon();
    if(!DirName->text().isEmpty()){
        DirName->clear();
    }
    //deleteLater();
    closeWidget();
}

void AddDirWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(230, 245, 230, 200));

    QPolygon trianglePolygon_Left;
    trianglePolygon_Left << QPoint(SHADOW_WIDTH_1 + X_offset, m_startX + m_triangleWidth / 2 + Y_offset);
    trianglePolygon_Left << QPoint(SHADOW_WIDTH_1 + m_triangleHeight + X_offset, m_startX + Y_offset);
    trianglePolygon_Left << QPoint(SHADOW_WIDTH_1 + m_triangleHeight + X_offset, m_startX + m_triangleWidth + Y_offset);

    QRect rect = QRect(m_triangleHeight + SHADOW_WIDTH_1 + X_offset,
                            SHADOW_WIDTH_1 + Y_offset,
                            m_width - SHADOW_WIDTH_1 * 2 - m_triangleHeight,
                            m_height - SHADOW_WIDTH_1 * 2);  //Rect

    QPainterPath drawPath;
    drawPath.addRoundedRect(rect,
                            BORDER_RADIUS_1, BORDER_RADIUS_1);  //Rect

    drawPath.addPolygon(trianglePolygon_Left);  //Triangle;
    painter.drawPath(drawPath);
}

bool AddDirWidget::event(QEvent *event)  //消去Popup自带阴影
{
    static bool class_amended = false;
    if(event->type() == QEvent::WinIdChange){
        HWND hwnd = reinterpret_cast<HWND>(winId());
        if (class_amended == false){
            class_amended = true;
            DWORD class_style = ::GetClassLong(hwnd, GCL_STYLE);
            class_style &= ~static_cast<unsigned long>(CS_DROPSHADOW);
            ::SetClassLong(hwnd, GCL_STYLE, static_cast<long>(class_style));
        }
    }else if(event->type() == QEvent::Show){
        emit highlightButtonIcon();
    }

    return QWidget::event(event);
}

void AddDirWidget::mousePressEvent(QMouseEvent *event)
{
    if(timer->remainingTime() > 0) return;
    if(!QRect(m_triangleHeight + SHADOW_WIDTH_1 + X_offset,
              SHADOW_WIDTH_1 + Y_offset,
              m_width - SHADOW_WIDTH_1 * 2 - m_triangleHeight,
              m_height - SHADOW_WIDTH_1 * 2).contains(event->pos())){
        emit restoreButtonIcon();
        //deleteLater();
        closeWidget();
    }
}

void AddDirWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Return:
        ConfirmButton->click();
        break;
    default:
        break;
    }
    QWidget::keyPressEvent(event);  //调用父类保证其他按键正常
}

void AddDirWidget::closeWidget()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(100);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    connect(animation, &QPropertyAnimation::finished, this, &AddDirWidget::deleteLater);
}
