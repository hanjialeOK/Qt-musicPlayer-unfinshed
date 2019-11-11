#include "widget_Bar/musicinfo_bar.h"

MusicInfoBar::MusicInfoBar(QWidget *parent) :
    QWidget(parent)
{
    initUi();
}

MusicInfoBar::~MusicInfoBar()
{
    delete TitleButton;
    delete AuthorButton;
    delete LikeOrNotButton;
    delete ShareButton;
    delete MaximizeButton;
    delete ImageLabel;
}

void MusicInfoBar::initUi()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    setFixedSize(250, 70);

    installEventFilter(this);  //for Enter and Leave

    QFrame *Frame = new QFrame(this);
    Frame->setGeometry(0, 0, 250, 70);
    Frame->setStyleSheet("QFrame{border-top:1px solid rgb(200, 200, 200); border-right:1px solid rgb(200, 200, 200); border-bottom:0px; border-left:0px;}");

    TitleButton = new QPushButton(this);
    TitleButton->setCursor(Qt::PointingHandCursor);
    TitleButton->setFlat(true);
    TitleButton->setFixedSize(60, 20);
    TitleButton->setText("未知作者");
    TitleButton->setFont(QFont("Microsoft YaHei", 9, QFont::Normal));
    TitleButton->setStyleSheet(
                "QPushButton{color:rgba(10, 10, 10, 200); border:0px solid rgb(0, 0, 0); text-align:left center;}"
                "QPushButton:hover{color:rgba(0, 0, 0, 255);}"
                "QPushButton:pressed{padding-left:1px; padding-top:1px;}"
                );
    TitleButton->setFocusPolicy(Qt::NoFocus);

    AuthorButton = new QPushButton(this);
    AuthorButton->setCursor(Qt::PointingHandCursor);
    AuthorButton->setFlat(true);
    AuthorButton->setFixedSize(60, 20);
    AuthorButton->setText("未知专辑");
    AuthorButton->setFont(QFont("Microsoft YaHei", 9, QFont::Normal));
    AuthorButton->setStyleSheet(
                "QPushButton{color:rgb(80, 80, 80); border:0px solid rgb(0, 0, 0); text-align:left center;}"
                "QPushButton:hover{color:rgb(20, 20, 20);}"
                "QPushButton:pressed{padding-left:1px; padding-top:1px;}"
                );
    AuthorButton->setFocusPolicy(Qt::NoFocus);

    LikeOrNotButton = new QPushButton(this);
    LikeOrNotButton->setCursor(Qt::PointingHandCursor);
    LikeOrNotButton->setFixedSize(20, 20);
    LikeOrNotButton->setIconSize(QSize(18, 18));
    LikeOrNotButton->setIcon(QIcon(":/musicInfoBar_Icon/like.png"));
    LikeOrNotButton->setStyleSheet("QPushButton{border:0px solid rgb(0, 0, 0);}");
    LikeOrNotButton->setFocusPolicy(Qt::NoFocus);
    LikeOrNotButton->installEventFilter(this);

    ShareButton = new QPushButton(this);
    ShareButton->setCursor(Qt::PointingHandCursor);
    ShareButton->setFixedSize(20, 20);
    ShareButton->setIconSize(QSize(20, 20));
    ShareButton->setIcon(QIcon(":/musicInfoBar_Icon/share.png"));
    ShareButton->setStyleSheet("QPushButton{border:0px solid rgb(0, 0, 0);}");
    ShareButton->setFocusPolicy(Qt::NoFocus);
    ShareButton->installEventFilter(this);

    MaximizeButton = new QPushButton(this);
    MaximizeButton->setCursor(Qt::PointingHandCursor);
    MaximizeButton->setFixedSize(52, 52);
    MaximizeButton->setIconSize(QSize(35, 35));
    MaximizeButton->setIcon(QIcon(":/musicInfoBar_Icon/maximize.png"));
    MaximizeButton->setStyleSheet("QPushButton{border:0px; background-color:rgba(50, 50, 50, 100);}");
    MaximizeButton->setFocusPolicy(Qt::NoFocus);
    MaximizeButton->hide();

    ImageLabel = new QLabel(this);
    ImageLabel->setCursor(Qt::PointingHandCursor);
    ImageLabel->setFixedSize(52, 52);
    ImageLabel->setScaledContents(true);
    ImageLabel->setPixmap(QPixmap(":/musicInfoBar_Icon/unknown.png"));
    ImageLabel->setFocusPolicy(Qt::NoFocus);
    ImageLabel->installEventFilter(this);

    TitleButton->move(70, 13);
    AuthorButton->move(70, 37);
    LikeOrNotButton->move(220, 15);
    ShareButton->move(220, 40);
    MaximizeButton->move(9, 9);
    ImageLabel->move(9,9);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(245, 245, 247));
    setAutoFillBackground(true);
    setPalette(pal);
}

bool MusicInfoBar::eventFilter(QObject *obj, QEvent *event)
{
    switch(event->type()){
    case QEvent::HoverEnter:
        if(obj == ShareButton){
            ShareButton->setIcon(QIcon(":/musicInfoBar_Icon/share_hover.png"));
            return true;
        }
        break;
    case QEvent::HoverLeave:
        if(obj == ShareButton){
            ShareButton->setIcon(QIcon(":/musicInfoBar_Icon/share.png"));
            return true;
        }
        break;
    case QEvent::Enter:
        if(obj == this){
            MaximizeButton->setToolTip("展开音乐详情页");
            MaximizeButton->show();
            MaximizeButton->raise();  //控制最大化按钮置顶显示
            return true;
        }
        break;
    case QEvent::Leave:
        if(obj == this){
            MaximizeButton->hide();
            return true;
        }
        break;
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}

void MusicInfoBar::updateMusicInfo(const QString &Title, const QString &Author, const QPixmap &Picture)
{
    TitleButton->setText(Title);
    TitleButton->setToolTip(Title);
    AuthorButton->setText(Author);
    AuthorButton->setToolTip(Author);
    ImageLabel->setPixmap(Picture);

    //QFontMetrics fontWidth(TitleButton->font());

    //int TitleSize = TitleButton->fontMetrics().width(Title);  //字符超长检测
    int TitleSize = QFontMetrics(TitleButton->font()).width(Title);
    if(TitleSize <= 140){
        TitleButton->setFixedWidth(TitleSize);
    }
    else{
        //QString Elide_title = TitleButton->fontMetrics().elidedText(Title, Qt::ElideRight, 140);//最大宽度140像素
        QString Elide_title = QFontMetrics(TitleButton->font()).elidedText(Title, Qt::ElideRight, 140);
        TitleButton->setFixedWidth(140);
        TitleButton->setText(Elide_title);
    }

    int AuthorSize = AuthorButton->fontMetrics().width(Author);  //字符超长检测
    if(AuthorSize <= 140){
        AuthorButton->setFixedWidth(AuthorSize);
    }
    else{
        QString Elide_Author = AuthorButton->fontMetrics().elidedText(Author, Qt::ElideRight, 140);//最大宽度140像素
        AuthorButton->setFixedWidth(140);
        AuthorButton->setText(Elide_Author);
    }
}
