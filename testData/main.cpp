#include "consolewindow.h"
#include "core.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ConsoleWindow window;
    Core core;
    window.setCoreLogic(core);
    window.show();

    return a.exec();
}
