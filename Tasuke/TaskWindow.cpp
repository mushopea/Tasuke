#include <QApplication>
#include <QDesktopWidget>
#include "Tasuke.h"
#include "TaskWindow.h"

TaskWindow::TaskWindow(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);

	//context menu actions
	quitAction = new QAction("&Quit", this);
	showInputWindowAction = new QAction("&Show Command Box", this);
	showTaskWindowAction = new QAction("&Show Display Window", this);
	settingsAction = new QAction("&Settings", this);
	helpAction = new QAction("&Help", this);
	aboutAction = new QAction("&About Tasuke", this);

	
	// uncomment when ready
	//setWindowFlags(Qt::Window | Qt::FramelessWindowHint);	
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
	center.setX(2 * QApplication::desktop()->screen()->rect().width() / 3);

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

void TaskWindow::contextMenuOperations(){
	
	//tray stuff
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(quitAction);
	trayIconMenu->addAction(showTaskWindowAction);
	trayIconMenu->addAction(showInputWindowAction);
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QIcon(":/Images/Traysuke.png"));
	trayIcon->show();

	//connect context menu actions
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(showTaskWindowAction, SIGNAL(triggered()), this, SLOT(showAndMoveToSide()));
	connect(showInputWindowAction, SIGNAL(triggered()), &(Tasuke::instance().getInputWindow()), SLOT(showAndCenter()));

	//when tray icon is clicked..
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

}