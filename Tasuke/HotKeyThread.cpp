#include <QSysInfo>
#include "HotKeyThread.h"

#ifdef Q_OS_WIN

#include <process.h>

#define MAGIC_NUM 1128
#define WM_END_THREAD (WM_USER+2)

HotKeyThread::HotKeyThread(QObject *parent) : QThread(parent) {
	qRegisterMetaType<KeyCombination>("KeyCombination");
	pid = 0;
}

HotKeyThread::~HotKeyThread() {

}

void HotKeyThread::stop() {
	if (pid != 0) {
		::PostThreadMessage(pid, WM_END_THREAD, 0, 0);
	}
}

void HotKeyThread::run() {
	pid = ::GetCurrentThreadId();
	ATOM id = ::GlobalAddAtom(MAKEINTATOM(MAGIC_NUM));
	ATOM id2 = ::GlobalAddAtom(MAKEINTATOM(MAGIC_NUM+1));
	ATOM id3 = ::GlobalAddAtom(MAKEINTATOM(MAGIC_NUM+2));
	::RegisterHotKey(NULL, id, MOD_CONTROL | MOD_NOREPEAT, VK_SPACE);
	::RegisterHotKey(NULL, id2, MOD_ALT | MOD_NOREPEAT, VK_SPACE);
	::RegisterHotKey(NULL, id3, MOD_ALT | MOD_CONTROL | MOD_NOREPEAT, VK_SPACE);

	MSG msg = {0};
	while (::GetMessage(&msg, NULL, 0, 0)) {
		if (msg.message == WM_HOTKEY) {
			int keyCode = msg.lParam;
			int key = keyCode >> 16;
			int mod = keyCode & 0xFFFF;

			if (mod == MOD_CONTROL && key == VK_SPACE) {
				emit hotKeyPress(KeyCombination::CTRL_SPACE);
			} else if (mod == MOD_ALT && key == VK_SPACE) {
				emit hotKeyPress(KeyCombination::ALT_SPACE);
			} else if (mod == (MOD_CONTROL | MOD_ALT) && key == VK_SPACE) {
				emit hotKeyPress(KeyCombination::CTRL_ALT_SPACE);
			}
		} else if (msg.message == WM_END_THREAD) {
			break;
		}
	}

	::UnregisterHotKey(NULL, id);
	::UnregisterHotKey(NULL, id2);
	::UnregisterHotKey(NULL, id3);
	::GlobalDeleteAtom(id);
	::GlobalDeleteAtom(id2);
	::GlobalDeleteAtom(id3);
}

#endif

#ifdef Q_OS_MAC

#include "MacWindowActivator.h"
#include "Tasuke.h"

HotKeyThread::HotKeyThread(QObject *parent) : QThread(parent) {
	qRegisterMetaType<KeyCombination>("KeyCombination");

}

HotKeyThread::~HotKeyThread() {

}

void HotKeyThread::stop() {

}

void HotKeyThread::run() {
    //handler
    hotKeyFunction = NewEventHandlerUPP(hotKeyHandler);
    EventTypeSpec eventType;
    eventType.eventClass = kEventClassKeyboard;
    eventType.eventKind = kEventHotKeyReleased;
    InstallApplicationEventHandler(hotKeyFunction,1,&eventType,this,NULL);
    //hotkey
    UInt32 keyCode = kVK_Space;
    EventHotKeyRef theRef = NULL;
    EventHotKeyID keyID;
    keyID.signature = 123; //arbitrary string
    keyID.id = 1;
    RegisterEventHotKey(keyCode, controlKey, keyID,GetApplicationEventTarget(), 0, &theRef);
}

OSStatus HotKeyThread::hotKeyHandler(EventHandlerCallRef nextHandler,EventRef theEvent, void *userData) {
    WindowActivator::activate();
    Tasuke::instance().getHotKeyManager().handleHotKeyPress(KeyCombination::CTRL_SPACE);
    return noErr;
}

#endif

#ifdef Q_OS_UNIX
#ifndef Q_OS_MAC
#error your os is not supported
#endif
#endif