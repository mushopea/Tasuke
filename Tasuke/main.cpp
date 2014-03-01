#include "TaskWindow.h"
#include <QtWidgets/QApplication>
#include <windows.h>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	
	TaskWindow taskWindow;
	taskWindow.show();

	Tasuke::instance();

	return app.exec();
}
