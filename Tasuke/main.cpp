#include "TaskWindow.h"
#include "InputWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	
	TaskWindow taskWindow;
	taskWindow.show();

	// TODO: make window appear on hotkey
	// TODO: make window disappear when not needed
	InputWindow inputWindow;
	inputWindow.show();

	return app.exec();
}
