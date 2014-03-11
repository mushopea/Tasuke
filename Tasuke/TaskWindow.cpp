#include <QApplication>
#include <QDesktopWidget>
#include "Tasuke.h"
#include "TaskWindow.h"

TaskWindow::TaskWindow(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);

	//setAttribute(Qt::WA_TranslucentBackground);
	//ui.closeButton->setStyleSheet("background-color: white; border: none;");
	//ui.minButton->setStyleSheet("background-color: white; border: none; QPushButton:hover { color:red;}");

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

	//context menu actions
	quitAction = new QAction("&Quit", this);
	showInputWindowAction = new QAction("&Show Command Box", this);
	showTaskWindowAction = new QAction("&Show Display Window", this);
	settingsAction = new QAction("&Settings", this);
	helpAction = new QAction("&Help", this);
	showAboutWindowAction = new QAction("&About Tasuke", this);
}

TaskWindow::~TaskWindow() {
	if (trayIcon) {
		delete trayIcon;
	}
	if (trayIconMenu) {
		delete trayIconMenu;
	}
	if (quitAction) {
		delete quitAction;
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
	ui.tableWidget->setRowCount(0);

	for (int i=0; i<tasks.size(); i++) {
		ui.tableWidget->insertRow(i);
		ui.tableWidget->setItem(i, 0, new QTableWidgetItem(tasks[i].getDescription()));
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
	if (reason == QSystemTrayIcon::Trigger) {
		Tasuke::instance().showInputWindow();	
	}

	if(reason == QSystemTrayIcon::DoubleClick){
		Tasuke::instance().showTaskWindow();
	}
}

//controls the actions of the context menu of the tray icon
//IIRC, this function is not in Tasuke.cpp because it is not a QObject...
//...And TaskWindow is the main QObject. :P
void TaskWindow::contextMenuOperations(){

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