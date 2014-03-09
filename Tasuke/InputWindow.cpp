#include <QApplication>
#include <QDesktopWidget>
#include "Tasuke.h"
#include "InputWindow.h"

InputWindow::InputWindow(QWidget* parent) : QWidget(parent) {
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background:transparent;");

	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);

	connect(ui.lineEdit, SIGNAL(returnPressed()), this, SLOT(handleReturnPressed()));
	connect(ui.lineEdit, SIGNAL(editingFinished()), this, SLOT(handleEditingFinished()));

	hotKeyThread = new HotKeyThread(this);
	connect(hotKeyThread, SIGNAL(hotKeyPress(int)), this, SLOT(handleHotKeyPress(int)), Qt::QueuedConnection);
	hotKeyThread->start();
}

InputWindow::~InputWindow() {

}

void InputWindow::showAndCenter() {
	QPoint center = QApplication::desktop()->screen()->rect().center() - rect().center();
	center.setY(QApplication::desktop()->screen()->rect().height() / 4);

	move(center);

	show();
	raise();
	activateWindow();
}

void InputWindow::closeEvent(QCloseEvent* event) {
	hotKeyThread->stop();
}

void InputWindow::handleReturnPressed() {
	std::string command = ui.lineEdit->text().toUtf8().constData();

	if (command.empty()) {
		return;
	}

	Tasuke::instance().runCommand(command);
}

void InputWindow::handleEditingFinished() {
	hide();
	ui.lineEdit->clear();
}

void InputWindow::handleHotKeyPress(int key) {
	if (isVisible() == true) {
		hide();
	} else {
		showAndCenter();
	}
}
