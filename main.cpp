#include "consolewindow.h"
#include "core.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ConsoleWindow window;
    window.show();

    try{
        Core core;
        core.oldMainTest("test.h"); 
    }
    catch(const std::exception& e)
    {
        window.consoleLog("Caught exception: '" + std::string(e.what()) + "'\n");
    }
    catch(...){
        window.consoleLog("Unpredictable error!\n");
    }

    return a.exec();
}
