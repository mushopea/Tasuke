//@author A0096836M

#ifndef HOTKEYTHREAD_H
#define HOTKEYTHREAD_H

#include <QtGlobal>
#include <QMetaType>
#include <QThread>

// Describes the keycombination pressed
enum class KeyCombination {
	CTRL_SPACE,
	ALT_SPACE,
	CTRL_ALT_SPACE
};

// Windows implementation using windows API
#ifdef Q_OS_WIN

// Do not define 'min()' and 'max()' macros breaking everything
#define NOMINMAX
// Do not define any windows API garbage in global scope
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

// HotKeyThread detects user hotkey press.
// Managed by HotKeyManager.
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

#endif

//@author A0096836M-unused
// Mac implementation using Carbon API
#ifdef Q_OS_MAC

#include <Carbon/Carbon.h>

// HotKeyThread detects user hotkey press.
// Managed by HotKeyManager.
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

#ifdef Q_OS_UNIX
#ifndef Q_OS_MAC
#error "Your os is not supported"
#endif
#endif

#endif
