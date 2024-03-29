#include "MainWindow.h"

#include "DeviceManagement/dvmainwindow.h"
#include "file_manage/filewindow.h"
#include "memory/memorysimulatewindow.h"
#include "process/Cpu.h"
#include "process/Process.h"
#include "process/ProcessWindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QTranslator       translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "simos_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    fileWindow file_window;
    file_window.show();
    ProcessWindow proc_window;
    proc_window.show();
    DVMainWindow device_window;
    device_window.show();
    MemorySimulateWindow mem_window;
    mem_window.show();

    return a.exec();
}
