#include "Tasuke.h"
#include "AboutWindow.h"

//@author A0100189

// This is a simple window that displays brief info about Tasuke
// And credits any resources used.
// It can be accessed in the tray icon context menu.

AboutWindow::AboutWindow(QDialog* parent) : QDialog(parent) {
	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint | Qt::Tool);
	ui.label_2->setText("V0.5"); // set version here.
}

AboutWindow::~AboutWindow() {
	
}

void AboutWindow::showAndCenter() {
	QPoint center = QApplication::desktop()->screen()->rect().center() - rect().center();
	center.setY(QApplication::desktop()->screen()->rect().height() / 4);

	move(center);

	show();
	raise();
	activateWindow();
}