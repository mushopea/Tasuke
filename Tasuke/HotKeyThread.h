#ifndef HOTKEYTHREAD_H
#define HOTKEYTHREAD_H

#include <QtGlobal>
#include <QMetaType>
#include <QThread>

enum class KeyCombination {
	CTRL_SPACE,
	ALT_SPACE,
	CTRL_ALT_SPACE
};

#ifdef Q_OS_WIN

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class HotKeyThread : public QThread {
	Q_OBJECT

public:
	HotKeyThread(QObject *parent);
	~HotKeyThread();

signals:
	void hotKeyPress(KeyCombination);

public slots:
	void run();
	void stop();
	
private:
	DWORD pid;
};

#else

#include <Carbon/Carbon.h>

class HotKeyThread : public QThread {
	Q_OBJECT

public:
	HotKeyThread(QObject *parent);
	~HotKeyThread();

signals:
	void hotKeyPress(KeyCombination);

public slots:
	void run();
	void stop();
	
private:
    EventHandlerUPP hotKeyFunction;
    static OSStatus hotKeyHandler(EventHandlerCallRef nextHandler, EventRef theEvent, void *userData);
};

#endif

#endif
