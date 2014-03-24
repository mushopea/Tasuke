#ifndef HOTKEYTHREAD_H
#define HOTKEYTHREAD_H

#include <QMetaType>
#include <QThread>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

enum class KeyCombination {
	CTRL_SPACE,
	ALT_SPACE
};

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