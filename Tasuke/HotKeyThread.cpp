//@author A0096836M

#include <QSysInfo>
#include "HotKeyThread.h"
#include "Constants.h"

#ifdef Q_OS_WIN

#include <process.h>

// randomly chosen
#define MAGIC_NUM 1128
#define WM_END_THREAD (WM_USER+2)

// Constructor for HotKeyThread. Takes in a parent object for memory hierachy.
// Defaults to null if parent not given.
HotKeyThread::HotKeyThread(QObject *parent) : QThread(parent) {
	qRegisterMetaType<KeyCombination>(METATYPE_KEYCOMIBNATION);
	pid = 0;
}

// Descrtuctor for HotKeyThread
HotKeyThread::~HotKeyThread() {

}

// Stop running the thread
void HotKeyThread::stop() {
	if (pid != 0) {
		::PostThreadMessage(pid, WM_END_THREAD, 0, 0);
	}
}

// Run the thread. This thread is detached and blocks with a busy-wait loop
void HotKeyThread::run() {
	pid = GetCurrentThreadId();

	ATOM idCtrlSpace = ::GlobalAddAtom(MAKEINTATOM(MAGIC_NUM));
	ATOM idAltSpace = ::GlobalAddAtom(MAKEINTATOM(MAGIC_NUM+1));
	ATOM idCtrlAltSpace = ::GlobalAddAtom(MAKEINTATOM(MAGIC_NUM+2));

	RegisterHotKey(NULL, idCtrlSpace, MOD_CONTROL | MOD_NOREPEAT, VK_SPACE);
	RegisterHotKey(NULL, idAltSpace, MOD_ALT | MOD_NOREPEAT, VK_SPACE);
	RegisterHotKey(NULL, idCtrlAltSpace, MOD_ALT | MOD_CONTROL | MOD_NOREPEAT, VK_SPACE);

	// busy-wait loop for hot key presses
	MSG msg = {0};
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (msg.message == WM_HOTKEY) {
			// receive hotkey signal

			// decode key
			int keyCode = msg.lParam;
			int key = keyCode >> 16;
			int mod = keyCode & 0xFFFF;

			// emit correct signals
			if (mod == MOD_CONTROL && key == VK_SPACE) {
				emit hotKeyPress(KeyCombination::CTRL_SPACE);
			} else if (mod == MOD_ALT && key == VK_SPACE) {
				emit hotKeyPress(KeyCombination::ALT_SPACE);
			} else if (mod == (MOD_CONTROL | MOD_ALT) && key == VK_SPACE) {
				emit hotKeyPress(KeyCombination::CTRL_ALT_SPACE);
			}
		} else if (msg.message == WM_END_THREAD) {
			// received signal to kill self 
			break;
		}
	}

	UnregisterHotKey(NULL, idCtrlSpace);
	UnregisterHotKey(NULL, idAltSpace);
	UnregisterHotKey(NULL, idCtrlAltSpace);

	GlobalDeleteAtom(idCtrlSpace);
	GlobalDeleteAtom(idAltSpace);
	GlobalDeleteAtom(idCtrlAltSpace);
}

#endif

//@author a0096836-unused
#ifdef Q_OS_MAC

#include "MacWindowActivator.h"
#include "Tasuke.h"

const int MAGIC_NUM = 147; // randomly chosen
const int KEY_ID = 1;

HotKeyThread::HotKeyThread(QObject *parent) : QThread(parent) {
	qRegisterMetaType<KeyCombination>(METATYPE_KEYCOMIBNATION);

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
    keyID.signature = MAGIC_NUM;
    keyID.id = KEY_ID;
    RegisterEventHotKey(keyCode, controlKey, keyID,GetApplicationEventTarget(), 0, &theRef);
}

OSStatus HotKeyThread::hotKeyHandler(EventHandlerCallRef nextHandler,EventRef theEvent, void *userData) {
    WindowActivator::activate();
    Tasuke::instance().getHotKeyManager().handleHotKeyPress(KeyCombination::CTRL_SPACE);
    return noErr;
}

#endif

// give error on unsupported os
#ifdef Q_OS_UNIX
#ifndef Q_OS_MAC
#error "Your os is not supported"
#endif

#endif