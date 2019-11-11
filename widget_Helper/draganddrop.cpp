#include "widget_Helper/draganddrop.h"

//********************thumbnail**********************thumbnail*********************thumbnail*******************thumbnail****************//

thumbnail::thumbnail(QWidget *parent) :
    QWidget(parent)
{
    initUi();
}

void thumbnail::initUi()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setFixedSize(140, 34);

    ImageButton = new QPushButton(this);
    ImageButton->setFixedSize(20, 20);
    ImageButton->setIconSize(QSize(20, 20));
    ImageButton->setFlat(true);
    ImageButton->setStyleSheet("QPushButton{border:0px;}");
    ImageButton->setFocusPolicy(Qt::NoFocus);
    ImageButton->move(RADIUS - RECT, RADIUS - RECT);

    TextLabel = new QLabel(this);
    TextLabel->setFixedSize(TEXT_LENGTH, 20);
    TextLabel->setAlignment(Qt::AlignVCenter);
    TextLabel->setFont(QFont("Microsoft YaHei", 8, QFont::Normal));
    TextLabel->setStyleSheet("QLabel{color:rgba(255, 255, 255, 255); border:0px solid rgb(0, 0, 0);}");
    TextLabel->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->addWidget(TextLabel);
    layout->setContentsMargins(2 * RADIUS, 0, 0, 2);
}

void thumbnail::setupthumbnail(QIcon icon, QString text)
{
    ImageButton->setIcon(QIcon(icon));

    TextLabel->setText(text);
    int textSize = fontMetrics().width(text);  //字符超长检测
    if(textSize > TEXT_LENGTH){
        QString Elide_text = fontMetrics().elidedText(text, Qt::ElideRight, TEXT_LENGTH);
        TextLabel->setText(Elide_text);
    }
}

void thumbnail::setIconSize(int size)
{
    ImageButton->setIconSize(QSize(size, size));
}

void thumbnail::paintEvent(QPaintEvent *)
{
    QPainter Painter(this);
    Painter.setRenderHint(QPainter::Antialiasing, true);
    Painter.setPen(Qt::NoPen);
    Painter.setBrush(QColor(114, 164, 255, 200));

    QPainterPath PainterPath;
    PainterPath.addRoundedRect(QRect(0, 0, width(), height()), RADIUS, RADIUS);  //Rect
    PainterPath.addEllipse(RADIUS - ELLIPSE_RADIUS, RADIUS - ELLIPSE_RADIUS, ELLIPSE_RADIUS * 2, ELLIPSE_RADIUS * 2);  //除去内部小圆
    Painter.drawPath(PainterPath);
//    Painter.drawRoundedRect(0, 0, width(), height(), height()/2, height()/2);
//    Painter.drawEllipse(5, 5, 24, 24);

    Painter.setBrush(QColor(255, 255, 255, 200));
    Painter.drawEllipse(RADIUS - ELLIPSE_RADIUS, RADIUS - ELLIPSE_RADIUS, ELLIPSE_RADIUS * 2, ELLIPSE_RADIUS * 2);  //内部小圆重新上色
//    Painter.drawPath(PainterPath);
//    QPainterPath PainterPath_Ellipse;
//    PainterPath_Ellipse.addEllipse(5, 5, 24, 24);
//    Painter.drawPath(PainterPath_Ellipse);
}

