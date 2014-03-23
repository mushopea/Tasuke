#ifndef HOTKEYTHREAD_H
#define HOTKEYTHREAD_H

#include <QThread>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class HotKeyThread : public QThread {
	Q_OBJECT

public:
	HotKeyThread(QObject *parent);
	~HotKeyThread();

signals:
	void hotKeyPress(int);

public slots:
	void run();
	void stop();
	
private:
	DWORD pid;
};

#endif