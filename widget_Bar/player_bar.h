#ifndef PLAYER_BAR_H
#define PLAYER_BAR_H

#include <QWidget>
#include <QIcon>
#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QEvent>
#include <QSlider>
#include <QMouseEvent>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QTime>
#include <QToolTip>

class CustomSlider;

class PlayerBar : public QWidget
{

#define Circle   0
#define Repeat   1
#define Random_   2
#define Sequence_ 3

    Q_OBJECT

public:
    explicit PlayerBar(QWidget *parent = nullptr);
    ~PlayerBar();

public:
    QMediaPlayer::State state() const;

private:
    QFrame *frame = nullptr;
    QLabel *TimeProgressLabel = nullptr;
    QLabel *TimeDurationLabel = nullptr;
    QPushButton *LastOneButton = nullptr;
    QPushButton *BeginOrPauseButton = nullptr;
    QPushButton *NextOneButton = nullptr;
    CustomSlider *ProgressBarSlider = nullptr;
    QPushButton *SoundVolumeButton = nullptr;
    QPushButton *PlayerPatternButton = nullptr;
    QPushButton *PlaylistButton = nullptr;
    QPushButton *SongsCountButton = nullptr;
    CustomSlider *SoundVolumeSlider = nullptr;
    bool is_MusicListButton_pressed = false;
    int VioceVolume = 50;
    int PlayerPattern_Helper = 0;
    qint64 m_duration = 240;
    QMediaPlayer::State playerState = QMediaPlayer::StoppedState;

private:
    void initUi();
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void setState(QMediaPlayer::State state);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void SetNumforSongsCountButton(int count);

private slots:
    void ShowVioceVolumeState();
    void Handle_Button_CLicked();
    void send_CustomSlider_Clicked();
    void updateDurationInfo(int value);
//    void setPosition();
//    void myPlayerSlot(QModelIndex index);
//    void on_importBtn_clicked();
//    void on_pushButton_2_clicked();

signals:
    void play();
    void pause();
    void next();
    void previous();
    void changeVolume(int volume);
    void CustomSlider_Clicked(int value);
    void setMediaPlaybackMode(QMediaPlaylist::PlaybackMode mode);
};

//************************CustomSlider类*************************CustomSlider类**************************CustomSlider类*********************//

class CustomSlider : public QSlider
{
    Q_OBJECT

public:
    explicit CustomSlider(QWidget *parent = nullptr);
    ~CustomSlider();

private:
    void mousePressEvent(QMouseEvent *event);

signals:
    void CustomSliderClicked();
};

#endif // PLAYER_BAR_H
