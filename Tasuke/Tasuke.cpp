#include <string>
#include <iostream>
#include <glog/logging.h>
#include <QMessageBox>
#include <QtGui\QFontDatabase>
#include "Constants.h"
#include "Exceptions.h"
#include "Interpreter.h"
#include "Commands.h"
#include "Tasuke.h"

// Constructor for the Tasuke singleton.
Tasuke::Tasuke(QWidget* parent) : QWidget(parent) {
	LOG(INFO) << "Tasuke object created";

	storage = new Storage();
	storage->loadFile();
	
	initialize();
}

// Destructor for the Tasuke singleton.
Tasuke::~Tasuke() {
	LOG(INFO) << "Tasuke object destroyed";
	
	if (hotKeyThread != nullptr) {
		hotKeyThread->stop();
		delete hotKeyThread;
	}

	if (storage != nullptr) {
		delete storage;
	}
}

void Tasuke::loadFonts(){
	LOG(INFO) << "Loading fonts";

	QFontDatabase fontDatabase; 
	fontDatabase.addApplicationFont(":/Fonts/fonts/Quicksand_Book.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/Quicksand_Book_Oblique.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/Quicksand_Light.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/Quicksand_Light_Oblique.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/Quicksand_Bold.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/Quicksand_Bold_Oblique.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/PrintClearly.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/PrintBold.otf");
}

void Tasuke::initialize(){
	loadFonts();
	updateTaskWindow(storage->getTasks());
	showTaskWindow();
	contextMenuOperations();

	hotKeyThread = new HotKeyThread(this);
	connect(hotKeyThread, SIGNAL(hotKeyPress(int)), this, SLOT(handleHotKeyPress(int)), Qt::QueuedConnection);
	hotKeyThread->start();
}

// Static method that returns the sole instance of Tasuke.
Tasuke& Tasuke::instance() {
	static Tasuke *instance = 0;
	
	if(instance == 0){
		instance = new Tasuke();
		return *instance;
	} else {
		return *instance;
	}
}

void Tasuke::handleHotKeyPress(int key) {
	LOG(INFO) << "Hot key pressed with keycode " << key;

	int kkey = key >> 16;
	int mod = key & 0xFFFF;

	if (mod == MOD_CONTROL && kkey == VK_SPACE) {
		if (inputWindow.isVisible() == true) {
			inputWindow.hide();
		} else {
			inputWindow.showAndCenter();
		}
	}
	else if (mod == MOD_ALT && kkey == VK_SPACE) {
		if (taskWindow.isVisible() == true) {
			taskWindow.hide();
		} else {
			taskWindow.showAndMoveToSide();
		}
	}

}

void Tasuke::handleIconActivated(QSystemTrayIcon::ActivationReason reason) {
	//Following shawn's advice to show both.
	if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
		showTaskWindow();
		showInputWindow();	
	}
}

//controls the actions of the context menu of the tray icon
void Tasuke::contextMenuOperations(){
	LOG(INFO) << "Install system tray";

	//context menu actions
	QAction* quitAction = new QAction("&Quit", this);
	QAction* showInputWindowAction = new QAction("Show &Command Box", this);
	QAction* showTaskWindowAction = new QAction("Show &Display Window", this);
	QAction* showSettingsWindowAction = new QAction("&Settings", this);
	QAction* showHelpWindowAction = new QAction("&Help", this);
	QAction* showAboutWindowAction = new QAction("&About Tasuke", this);

	//tray stuff
	QMenu* trayIconMenu = new QMenu(this);
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
	connect(showTaskWindowAction, SIGNAL(triggered()), this, SLOT(showTaskWindow()));
	connect(showInputWindowAction, SIGNAL(triggered()), this, SLOT(showInputWindow()));
	connect(showAboutWindowAction, SIGNAL(triggered()),  this, SLOT(showAboutWindow()));

	//when tray icon is clicked..
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(handleIconActivated(QSystemTrayIcon::ActivationReason)));
}

void Tasuke::setStorage(IStorage* _storage) {
	LOG(INFO) << "Storage changed";

	storage = _storage;
}

InputWindow& Tasuke::getInputWindow(){
	return inputWindow;
}

AboutWindow& Tasuke::getAboutWindow(){
	return aboutWindow;
}

TaskWindow& Tasuke::getTaskWindow(){
	return taskWindow;
}

// This function exposes the Storage instance for editing.
IStorage& Tasuke::getStorage() {
	if (storage == nullptr) {
		throw ExceptionNullPtr();
	}
	return *storage;
}

void Tasuke::showInputWindow() {
	inputWindow.showAndCenter();
}

void Tasuke::showTaskWindow() {
	taskWindow.showAndMoveToSide();
}

void Tasuke::showAboutWindow(){
	aboutWindow.showAndCenter();
}

void Tasuke::hideTaskWindow() {
	taskWindow.hide();
}

void Tasuke::showMessage(QString message) {
	LOG(INFO) << "Showing message: " << message.toStdString();

	trayIcon->showMessage("Tasuke", message);
}

void Tasuke::updateTaskWindow(QList<Task> tasks) {
	LOG(INFO) << "Updating task window with " << QString::number(tasks.size()).toStdString() << " tasks";

	taskWindow.showTasks(tasks);
}

// This function runs a command in a string
void Tasuke::runCommand(QString commandString) {
	try {
		std::shared_ptr<ICommand> command = std::shared_ptr<ICommand>(Interpreter::interpret(commandString));
		if (command == nullptr) {
			return;
		}
		command->run();

		LOG(INFO) << "Pushing command to history stack";
		commandUndoHistory.push_back(command);
		commandRedoHistory.clear();
	} catch (ExceptionBadCommand exception) {
		LOG(INFO) << "Error parsing command";
		
		showMessage("Error parsing command");
	}
}

void Tasuke::undoCommand() {
	if (commandUndoHistory.size() == 0) {
		showMessage("Nothing to undo");
		return;
	}

	LOG(INFO) << "Undoing command";
	std::shared_ptr<ICommand> command = commandUndoHistory.back();
	commandUndoHistory.pop_back();
	command->undo();
	commandRedoHistory.push_back(command);
}

void Tasuke::redoCommand() {
	if (commandRedoHistory.size() == 0) {
		// Nothing to redo
		return;
	}

	LOG(INFO) << "Redoing command";
	std::shared_ptr<ICommand> command = commandRedoHistory.back();
	commandRedoHistory.pop_back();
	command->run();
	commandUndoHistory.push_back(command);
}