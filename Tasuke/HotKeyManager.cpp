#include <glog/logging.h>
#include "Constants.h"
#include "Tasuke.h"
#include "HotKeyManager.h"

//@author A0096836M

// Constructor for HotKeyManager. Takes in a parent object for
// memory hierachy. Defaults to null.
HotKeyManager::HotKeyManager(QObject *parent) : QObject(parent) {
	LOG(INFO) << MSG_HOTKEYMANAGER_CREATED;

	hotKeyThread = new HotKeyThread(this);
	connect(hotKeyThread, SIGNAL(hotKeyPress(KeyCombination)), this, SLOT(handleHotKeyPress(KeyCombination)), Qt::QueuedConnection);
	hotKeyThread->start();
}

// Destructor for HotKeyManager
HotKeyManager::~HotKeyManager() {
	LOG(INFO) << MSG_HOTKEYMANAGER_DESTROYED;

	if (hotKeyThread != nullptr) {
		hotKeyThread->stop();
		delete hotKeyThread;
	}
}

// Method activates when HotKeyThread signals the manager
void HotKeyManager::handleHotKeyPress(KeyCombination key) {
	LOG(INFO) << MSG_HOTKEY_PRESSED(key);

	if (key == KeyCombination::CTRL_SPACE) {
		Tasuke::instance().toggleInputWindow();
	} else if (key == KeyCombination::ALT_SPACE) {
		Tasuke::instance().toggleTaskWindow();
	} else if (key == KeyCombination::CTRL_ALT_SPACE) {
		Tasuke::instance().toggleBothWindows();
	}
}