//@author A0096836M

#include <glog/logging.h>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include "Tasuke.h"
#include "SystemTrayWidget.h"

// Constructor for SystemTrayWidget. Takes in parent object for memory hierachy
// Parent defaults to null
SystemTrayWidget::SystemTrayWidget(QWidget *parent ) : QWidget(parent) {
	LOG(INFO) << MSG_SYSTEMTRAYWIDGET_CREATED;

	installTrayIcon();

	// clean up when quiting
	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
}

// Destructor for SystemTrayWidget. Automatically removes icon from system tray
SystemTrayWidget::~SystemTrayWidget() {
	LOG(INFO) << MSG_SYSTEMTRAYWIDGET_DESTROYED;

	trayIcon->hide();
	delete trayIcon;
	trayIcon = nullptr;
}

// Displays a message using the tray icon bubble
void SystemTrayWidget::showMessage(QString message) {
	trayIcon->showMessage(TASUKE, message);
}

// Activated when user clicks on corresponding menu item
void SystemTrayWidget::handleShowTaskWindow() {
	Tasuke::instance().showTaskWindow();
}

// Activated when user clicks on corresponding menu item
void SystemTrayWidget::handleShowInputWindow() {
	Tasuke::instance().showInputWindow();
}

// Activated when user clicks on corresponding menu item
void SystemTrayWidget::handleShowTutorialWidget() {
	Tasuke::instance().showTutorial();
}

// Activated when user clicks on corresponding menu item
void SystemTrayWidget::handleShowSettingsWindow() {
	Tasuke::instance().showSettingsWindow();
}

// Activated when user clicks on corresponding menu item
void SystemTrayWidget::handleShowAboutWindow() {
	Tasuke::instance().showAboutWindow();
}

// Activated when user clicks on the icon
void SystemTrayWidget::handleIconActivated(
	QSystemTrayIcon::ActivationReason reason) {
	if (reason == QSystemTrayIcon::Trigger
		|| reason == QSystemTrayIcon::DoubleClick) {
		Tasuke::instance().showTaskWindow();
		Tasuke::instance().showInputWindow();	
	}
}

// Activated when the app is about to quit
void SystemTrayWidget::handleAboutToQuit() {
	trayIcon->hide();
}

// Creates and install a tray icon + menu
void SystemTrayWidget::installTrayIcon() {
	// context menu actions
	// NOTE: the new do not need to managed because they are automatically
	// deleted by Qt's memory hierachy when the parent (passed to the constructor)
	// are deleted
	QAction* quitAction = new QAction(TRAY_MENU_QUIT, this);
	QAction* showInputWindowAction = new QAction(TRAY_MENU_INPUT, this);
	QAction* showTaskWindowAction = new QAction(TRAY_MENU_TASK, this);
	QAction* showSettingsWindowAction = new QAction(TRAY_MENU_SETTINGS, this);
	QAction* showTutorialWidgetAction = new QAction(TRAY_MENU_TUTORIAL, this);
	QAction* showAboutWindowAction = new QAction(TRAY_MENU_ABOUT, this);

	// create menu for tray icon
	QMenu* trayIconMenu = new QMenu(this);
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QIcon(TRAY_ICON_PATH));
	trayIcon->show();

	// add created actions
	trayIconMenu->addAction(showTaskWindowAction);
	trayIconMenu->addAction(showInputWindowAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(showSettingsWindowAction);
	trayIconMenu->addAction(showTutorialWidgetAction);
	trayIconMenu->addAction(showAboutWindowAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);
	
	// connect actions
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(showTaskWindowAction, SIGNAL(triggered()),
		this, SLOT(handleShowTaskWindow()));
	connect(showInputWindowAction, SIGNAL(triggered()),
		this, SLOT(handleShowInputWindow()));
	connect(showTutorialWidgetAction, SIGNAL(triggered()),
		this, SLOT(handleShowTutorialWidget()));
	connect(showSettingsWindowAction, SIGNAL(triggered()), 
		this, SLOT(handleShowSettingsWindow()));
	connect(showAboutWindowAction, SIGNAL(triggered()), 
		this, SLOT(handleShowAboutWindow()));

	// connect tray icon clicked
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(handleIconActivated(QSystemTrayIcon::ActivationReason)));
}