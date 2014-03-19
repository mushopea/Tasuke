#include <glog/logging.h>
#include <QApplication>
#include <QDesktopWidget>
#include "Tasuke.h"
#include "TaskWindow.h"
#include "TaskEntry.h"
#include "qlistwidget.h"
//#include <qdatetime.h>
//#include <QList>

TaskWindow::TaskWindow(QWidget* parent) : QMainWindow(parent) {
	LOG(INFO) << "TaskWindow instance created";

	ui.setupUi(this);

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);

	//hotkey
	hotKeyThread = new HotKeyThread(this);
	connect(hotKeyThread, SIGNAL(hotKeyPress(int)), this, SLOT(handleHotKeyPress(int)), Qt::QueuedConnection);
	hotKeyThread->start();
}

TaskWindow::~TaskWindow() {
	LOG(INFO) << "TaskWindow instance destroyed";
}


void TaskWindow::showTasks(QList<Task> tasks) {
	ui.taskList->clear();
	
	for (int i = 0; i < tasks.size(); i++){

		Task t = tasks[i];

		TaskEntry * entry = new TaskEntry(i+1, t.getDescription(), t.getTags(), t.getBegin(), t.getEnd(), this);

		QListWidgetItem *listItem = new QListWidgetItem();
		listItem->setSizeHint(QSize(780,65));
		ui.taskList->addItem(listItem);
		ui.taskList->setItemWidget(listItem, entry);	
	}

}

void TaskWindow::showAndMoveToSide() {
	QPoint center = QApplication::desktop()->screen()->rect().center() - rect().center();
	center.setY(QApplication::desktop()->screen()->rect().height() / 9);

	move(center);

	showNormal();
	raise();
	activateWindow();
}


void TaskWindow::closeEvent(QCloseEvent *event) {
	hide();
	event->ignore();
}

void TaskWindow::mousePressEvent(QMouseEvent *event){
    mpos = event->pos();
}

void TaskWindow::mouseMoveEvent(QMouseEvent *event){
    if (event->buttons() && Qt::LeftButton) {
        QPoint diff = event->pos() - mpos;
        QPoint newpos = this->pos() + diff;

        this->move(newpos);
    }
}

void TaskWindow::keyDownEvent(QKeyEvent *event){

	event->accept();

	if ((event->modifiers() == Qt::CTRL) && (event->key() == Qt::UpArrow)) {
		ui.scrollbox->scroll(0, -300);
	}
		
	if ((event->modifiers() == Qt::CTRL) && (event->key() == Qt::DownArrow)) {
		ui.scrollbox->scroll(0, 300);
	}

	if (event->key() == Qt::UpArrow) {
		ui.scrollbox->scroll(0,-60);
	}
		
	if (event->key() == Qt::DownArrow) {
		ui.scrollbox->scroll(0, 60);
	}
	
	ui.scrollbox->repaint();
}	



void TaskWindow::handleHotKeyPress(int key) {
	if (isVisible() == true) {
		hide();
	} else {
		showAndMoveToSide();
	}
}