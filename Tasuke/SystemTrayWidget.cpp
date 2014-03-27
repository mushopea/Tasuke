#include <glog/logging.h>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include "Tasuke.h"
#include "SystemTrayWidget.h"

SystemTrayWidget::SystemTrayWidget(QWidget *parent ) : QWidget(parent) {
	LOG(INFO) << "SystemTrayWidget object created";

	//context menu actions
	QAction* quitAction = new QAction("&Quit", this);
	QAction* showInputWindowAction = new QAction("Show &Command Box", this);
	QAction* showTaskWindowAction = new QAction("Show &Task Window", this);
	QAction* showSettingsWindowAction = new QAction("&Settings", this);
	QAction* showTutorialWidgetAction = new QAction("&Tutorial", this);
	QAction* showAboutWindowAction = new QAction("&About Tasuke", this);

	//tray stuff
	QMenu* trayIconMenu = new QMenu(this);
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QIcon(":/Images/images/Traysuke.png"));
	trayIcon->show();

	//add actions
	trayIconMenu->addAction(quitAction);
	trayIconMenu->addAction(showTaskWindowAction);
	trayIconMenu->addAction(showInputWindowAction);
	trayIconMenu->addAction(showSettingsWindowAction);
	trayIconMenu->addAction(showTutorialWidgetAction);
	trayIconMenu->addAction(showAboutWindowAction);

	//connect context menu actions
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(showTaskWindowAction, SIGNAL(triggered()), this, SLOT(handleShowTaskWindow()));
	connect(showInputWindowAction, SIGNAL(triggered()), this, SLOT(handleShowInputWindow()));
	connect(showTutorialWidgetAction, SIGNAL(triggered()),  this, SLOT(handleShowTutorialWidget()));
	connect(showSettingsWindowAction, SIGNAL(triggered()),  this, SLOT(handleShowSettingsWindow()));
	connect(showAboutWindowAction, SIGNAL(triggered()),  this, SLOT(handleShowAboutWindow()));

	//when tray icon is clicked..
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(handleIconActivated(QSystemTrayIcon::ActivationReason)));

	//when quiting
	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
}

SystemTrayWidget::~SystemTrayWidget() {
	LOG(INFO) << "SystemTrayWidget object destroyed";

	trayIcon->hide();
	delete trayIcon;
	trayIcon = nullptr;
}

void SystemTrayWidget::showMessage(QString message) {
	trayIcon->showMessage("Tasuke", message);
}

void SystemTrayWidget::handleShowTaskWindow() {
	Tasuke::instance().showTaskWindow();
}

void SystemTrayWidget::handleShowInputWindow() {
	Tasuke::instance().showInputWindow();
}

void SystemTrayWidget::handleShowTutorialWidget() {
	Tasuke::instance().showTutorial();
}

void SystemTrayWidget::handleShowSettingsWindow() {
	Tasuke::instance().showSettingsWindow();
}

void SystemTrayWidget::handleShowAboutWindow() {
	Tasuke::instance().showAboutWindow();
}

void SystemTrayWidget::handleIconActivated(QSystemTrayIcon::ActivationReason reason) {
	if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
		Tasuke::instance().showTaskWindow();
		Tasuke::instance().showInputWindow();	
	}
}

void SystemTrayWidget::handleAboutToQuit() {
	trayIcon->hide();
}