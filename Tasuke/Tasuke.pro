include(Tasuke.pri)

TEMPLATE = app
TARGET = Tasuke
QT += core widgets gui
CONFIG += qt static release

DEPENDPATH += .
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

CONFIG(release, debug|release) {
    DESTDIR = ../bin/release
    INCLUDEPATH += ./GeneratedFiles \
        . \
        ./GeneratedFiles/release
    MOC_DIR += ./GeneratedFiles/release
    OBJECTS_DIR += release
} else {
    DESTDIR = ../bin/debug
    INCLUDEPATH += ./GeneratedFiles \
        . \
        ./GeneratedFiles/debug
    MOC_DIR += ./GeneratedFiles/debug
    OBJECTS_DIR += debug
}

win32 {
    LIBS += -L$$_PRO_FILE_PWD_/../Win32/Release -llibglog
    LIBS += -L$$_PRO_FILE_PWD_/../Win32/Release -llibhunspell
    INCLUDEPATH += $$_PRO_FILE_PWD_/../hunspell-1.3.2/src
    INCLUDEPATH += $$_PRO_FILE_PWD_/../glog-0.3.3/src/windows
    DEFINES += GOOGLE_GLOG_DLL_DECL=
    DEFINES += HUNSPELL_STATIC
}

unix {
    LIBS += -L/usr/local/lib -L/usr/lib -llibglog
    LIBS += -L/usr/local/lib -L/usr/lib -llibhunspell-1.3
    INCLUDEPATH += /usr/local/include \
        /usr/include
}

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
