#include "widget_Helper/mpeghelper.h"

MpegHelper::MpegHelper()
{}

bool MpegHelper::equal(QList<MP3_INFO> a, QList<MP3_INFO> b)
{
    if(a.size() != b.size()) return false;
    for(int i = 0; i < a.size(); i++){
        if(a.at(i).Url.url() != b.at(i).Url.url())
            return false;
    }
    return true;
}
