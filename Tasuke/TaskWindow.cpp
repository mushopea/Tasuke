#include <QApplication>
#include <QDesktopWidget>
#include "Tasuke.h"
#include "TaskWindow.h"

TaskWindow::TaskWindow(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);

	//declare actions
	quitAction = new QAction("&Quit", this);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	//tray actions
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(quitAction);
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QIcon(":/Images/Tasuke.png"));
	trayIcon->show();
	
	// uncomment when ready
	//setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	
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