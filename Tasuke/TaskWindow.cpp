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
   
	//context menu actions
	quitAction = new QAction("&Quit", this);
	showInputWindowAction = new QAction("&Show Command Box", this);
	showTaskWindowAction = new QAction("&Show Display Window", this);
	showSettingsWindowAction = new QAction("&Settings", this);
	showHelpWindowAction = new QAction("&Help", this);
	showAboutWindowAction = new QAction("&About Tasuke", this);

	contextMenuOperations();
}

TaskWindow::~TaskWindow() {
	LOG(INFO) << "TaskWindow instance destroyed";

	if (trayIcon) {
		delete trayIcon;
	}
	if (trayIconMenu) {
		delete trayIconMenu;
	}
	if (quitAction) {
		delete quitAction;
	}
	if (showInputWindowAction) {
		delete showInputWindowAction;
	}
	if (showTaskWindowAction){
		delete showTaskWindowAction;
	}
	if (showSettingsWindowAction){
		delete showSettingsWindowAction;
	}
	if (showHelpWindowAction){
		delete showHelpWindowAction;
	}
	if (showAboutWindowAction){
		delete showAboutWindowAction;
	}

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

void TaskWindow::showAndMoveToSide() {
	QPoint center = QApplication::desktop()->screen()->rect().center() - rect().center();
	center.setY(QApplication::desktop()->screen()->rect().height() / 9);

	move(center);

	showNormal();
	raise();
	activateWindow();
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

void TaskWindow::showMessage(QString message) {
	trayIcon->showMessage("Tasuke", message);
}

void TaskWindow::closeEvent(QCloseEvent *event) {
	if (trayIcon->isVisible()) {
		hide();
		event->ignore();
	}
}

void TaskWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
	//Following shawn's advice to show both.
	if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
		Tasuke::instance().showTaskWindow();
		Tasuke::instance().showInputWindow();	
	}
}

//controls the actions of the context menu of the tray icon
void TaskWindow::contextMenuOperations(){
	LOG(INFO) << "Install system tray";

	//tray stuff
	trayIconMenu = new QMenu(this);
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QIcon(":/Images/Traysuke.png"));
	trayIcon->show();

	//add actions
	trayIconMenu->addAction(quitAction);
	trayIconMenu->addAction(showTaskWindowAction);
	trayIconMenu->addAction(showInputWindowAction);
	trayIconMenu->addAction(showAboutWindowAction);

	//connect context menu actions
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(showTaskWindowAction, SIGNAL(triggered()), this, SLOT(showAndMoveToSide()));
	connect(showInputWindowAction, SIGNAL(triggered()), &(Tasuke::instance().getInputWindow()), SLOT(showAndCenter()));
	connect(showAboutWindowAction, SIGNAL(triggered()),  &(Tasuke::instance().getAboutWindow()), SLOT(showAndCenter()));

	//when tray icon is clicked..
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

}