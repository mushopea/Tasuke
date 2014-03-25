#include <glog/logging.h>
#include "Tasuke.h"
#include "HotKeyManager.h"

HotKeyManager::HotKeyManager(QObject *parent) : QObject(parent) {
	hotKeyThread = new HotKeyThread(this);
	connect(hotKeyThread, SIGNAL(hotKeyPress(KeyCombination)), this, SLOT(handleHotKeyPress(KeyCombination)), Qt::QueuedConnection);
	hotKeyThread->start();
}

HotKeyManager::~HotKeyManager() {
	if (hotKeyThread != nullptr) {
		hotKeyThread->stop();
		delete hotKeyThread;
	}
}

void HotKeyManager::handleHotKeyPress(KeyCombination key) {
	LOG(INFO) << "Hot key pressed with keycode " << (int)key;

	if (key == KeyCombination::CTRL_SPACE) {
		Tasuke::instance().toggleInputWindow();
	} else if (key == KeyCombination::ALT_SPACE) {
		Tasuke::instance().toggleTaskWindow();
	}
}