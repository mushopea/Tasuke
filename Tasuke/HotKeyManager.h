//@author A0096836M

#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include <QObject>
#include "HotKeyThread.h"

// Manages the hotkey thread and signals Tasuke about user hotkeys.
class HotKeyManager : public QObject {
	Q_OBJECT

public:
	HotKeyManager(QObject *parent = 0);
	~HotKeyManager();

public slots:
	void handleHotKeyPress(KeyCombination key);

private:
	HotKeyThread *hotKeyThread;
};

#endif // HOTKEYMANAGER_H
