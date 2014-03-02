#include "Tasuke.h"
#include "TaskWindow.h"

TaskWindow::TaskWindow(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);

	quitAction = new QAction("&Quit", this);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

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

}

void TaskWindow::showTasks(QList<Task> tasks) {
	ui.tableWidget->setRowCount(0);

	for (int i=0; i<tasks.size(); i++) {
		ui.tableWidget->insertRow(i);
		ui.tableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(i+1)));
		ui.tableWidget->setItem(i, 1, new QTableWidgetItem(tasks[i].getDescription()));
	}
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
}