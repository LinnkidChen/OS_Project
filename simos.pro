QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    simos/FileSystemSimulateWindow.cpp \
    simos/MemorySimulateWindow.cpp \
    simos/ProcessSimulateWindow.cpp \
    simos/main.cpp \
    simos/MainWindow.cpp

HEADERS += \
    simos/FileSystemSimulateWindow.h \
    simos/MainWindow.h \
    simos/MemorySimulateWindow.h \
    simos/ProcessSimulateWindow.h

FORMS += \
    ui/FileSystemSimulateWindow.ui \
    ui/MainWindow.ui \
    ui/MemorySimulateWindow.ui \
    ui/ProcessSimulateWindow.ui

TRANSLATIONS += \
    i18n/simos_zh_CN.ts

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .clang-format \
    .clang-tidy
