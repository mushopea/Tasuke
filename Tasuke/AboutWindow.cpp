#include <QApplication>
#include <QDesktopWidget>
#include "Tasuke.h"
#include "AboutWindow.h"

AboutWindow::AboutWindow(QDialog* parent) : QDialog(parent) {
	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint | Qt::Tool);
	
	ui.label_2->setText("V0.2");
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

/*
void AboutWindow::closeEvent(QCloseEvent* event) {

} */

