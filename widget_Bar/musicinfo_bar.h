#ifndef MUSICINFO_BAR_H
#define MUSICINFO_BAR_H

#include <QWidget>
#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QHash>

class MusicInfoBar : public QWidget
{
    Q_OBJECT

public:
    explicit MusicInfoBar(QWidget *parent = nullptr);
    ~MusicInfoBar();

public:
    QPushButton *TitleButton = nullptr;
    QPushButton *AuthorButton = nullptr;
    QLabel *ImageLabel = nullptr;
    QPushButton *MaximizeButton = nullptr;

private:
    QPushButton *LikeOrNotButton = nullptr;
    QPushButton *ShareButton = nullptr;

private:
    void initUi();
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void updateMusicInfo(const QString &Title, const QString &Author, const QPixmap &Picture);
};


#endif // MUSICINFO_BAR_H
