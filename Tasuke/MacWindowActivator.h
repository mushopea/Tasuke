#include <QApplication>

#ifndef Q_OS_MAC
#error You shoudn't be inlcuding this file
#endif

#ifndef MACWINDOWACTIVATOR_H
#define MACWINDOWACTIVATOR_H

class WindowActivator {
public:
    static void activate();
};

#endif
