#include <QSysInfo>
#include <process.h>
#include "HotKeyThread.h"

#define MAGIC_NUM 1128
#define WM_END_THREAD (WM_USER+2)

HotKeyThread::HotKeyThread(QObject *parent) : QThread(parent) {
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
	RegisterHotKey(NULL, id, MOD_CONTROL | MOD_NOREPEAT, VK_SPACE);

	MSG msg = {0};
	while (::GetMessage(&msg, NULL, 0, 0)) {
		if (msg.message == WM_HOTKEY) {
			emit hotKeyPress(msg.lParam);
		} else if (msg.message == WM_END_THREAD) {
			break;
		}
	}

	::UnregisterHotKey(NULL, id);
	::GlobalDeleteAtom(id);
}