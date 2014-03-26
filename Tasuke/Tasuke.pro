include(Tasuke.pri)

TEMPLATE = app
TARGET = Tasuke
DESTDIR = ../bin
QT += core widgets gui
CONFIG += qt

INCLUDEPATH += ./GeneratedFiles \
    . \
    ./GeneratedFiles/Debug

unix {
    LIBS += -L/usr/local/lib -L/usr/lib -lglog
    LIBS += -L/usr/local/lib -L/usr/lib -lhunspell-1.3
    INCLUDEPATH += /usr/local/include \
        /usr/include
}

DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

macx {
    QMAKE_CXXFLAGS = -mmacosx-version-min=10.7 -std=c++11 -stdlib=libc++
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    CONFIG += c++11
    ICON = Tasuke.icns
    QMAKE_LFLAGS += -framework carbon -framework cocoa
    HUNSPELL.files += ./en_GB.aff
    HUNSPELL.files += ./en_GB.dic
    HUNSPELL.files += ./en_US.aff
    HUNSPELL.files += ./en_US.dic
    HUNSPELL.path = Contents/Resources
    QMAKE_BUNDLE_DATA += HUNSPELL
}

win32:RC_FILE = Tasuke.rc
