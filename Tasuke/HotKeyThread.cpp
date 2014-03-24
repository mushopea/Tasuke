#include <QSysInfo>
#include <process.h>
#include "HotKeyThread.h"

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
	::RegisterHotKey(NULL, id, MOD_CONTROL | MOD_NOREPEAT, VK_SPACE);
	::RegisterHotKey(NULL, id2, MOD_ALT | MOD_NOREPEAT, VK_SPACE);

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
			}
		} else if (msg.message == WM_END_THREAD) {
			break;
		}
	}

	::UnregisterHotKey(NULL, id);
	::UnregisterHotKey(NULL, id2);
	::GlobalDeleteAtom(id);
	::GlobalDeleteAtom(id2);
}