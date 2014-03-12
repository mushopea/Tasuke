#include <QApplication>
#include <QDesktopWidget>
#include <QtGui\qbitmap.h>
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
	if(Tasuke::instance().getTaskWindow().hasFocus() || Tasuke::instance().getTaskWindow().isMinimized() == false){ //if taskWindow is open
		center.setY( (Tasuke::instance().getTaskWindow().y()) + Tasuke::instance().getTaskWindow().height() ); //set commandbox below taskWindow
	} else {
		center.setY(QApplication::desktop()->screen()->rect().height() / 4);
	}

	move(center);

	show();
	raise();
	activateWindow();
}

void InputWindow::closeEvent(QCloseEvent* event) {
	hotKeyThread->stop();
}


//will be updated when "themes" is implemented.
void InputWindow::changeBorder(int themeNumber){
	QPixmap pxr(QString::fromUtf8("InputWindowMask.png"));
	QPixmap pxr2(QString::fromUtf8("InputWindowMask.png"));
	pxr.fill(Qt::red);
	pxr.setMask( pxr2.createMaskFromColor( Qt::transparent ) );


    ui.label_2->setPixmap(pxr);
}

//will be updated when "themes" is implemented.
void InputWindow::changeBG(int themeNumber){
	//ui.label_2->setPixmap(pxr);
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
