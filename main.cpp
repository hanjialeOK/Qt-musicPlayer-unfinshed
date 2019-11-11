#include "main_window.h"
#include "test.h"
#include <QApplication>
//#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Main_Window *window = new Main_Window;
    window->show();
    return app.exec();
}
