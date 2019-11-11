#ifndef MUSICPLAYHELPER_H
#define MUSICPLAYHELPER_H

#include <QWidget>
#include <QUrl>

class MpegHelper
{
public:
    MpegHelper();

    typedef struct MP3_INFO
    {
        bool isEmpty = true;
        QString Path;        //存放这首歌的文件夹地址
        QUrl Url;            //存放这首歌的详细地址
        QString Title;            //歌名  TIT2
        QString Artist;          //歌手  TPE1
        QString Album;           //专辑  TALB
        int Duration;           //时长/单位秒
        QString str_Duration;   //时长/字符串格式
        QPixmap Pictrue;       //歌曲图片
        QString str_Size;        //文件大小/字符串格式
    }MP3_INFO;

    static bool equal(QList<MP3_INFO> a, QList<MP3_INFO> b);
};

#endif // MUSICPLAYHELPER_H
