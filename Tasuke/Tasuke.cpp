#include <string>
#include <iostream>
#include <glog/logging.h>
#include <QMessageBox>
#include <QFontDatabase>
#include "Constants.h"
#include "Exceptions.h"
#include "Interpreter.h"
#include "Commands.h"
#include "Tasuke.h"

bool Tasuke::guiMode = true;

// Constructor for the Tasuke singleton.
Tasuke::Tasuke() {
	LOG(INFO) << "Tasuke object created";

	spellObj = new Hunspell("en_GB.aff", "en_GB.dic");
	
	storage = new Storage();
	storage->loadFile();

	taskWindow = nullptr;
	inputWindow = nullptr;
	aboutWindow = nullptr;
	systemTrayWidget = nullptr;
	hotKeyManager = nullptr;

	if (guiMode) {
		initialize();
	}
}

// Destructor for the Tasuke singleton.
Tasuke::~Tasuke() {
	LOG(INFO) << "Tasuke object destroyed";

	if (hotKeyManager != nullptr) {
		delete hotKeyManager;
	}
	
	if (systemTrayWidget != nullptr) {
		delete systemTrayWidget;
	}

	if (aboutWindow != nullptr) {
		delete aboutWindow;
	}

	if (inputWindow != nullptr) {
		delete inputWindow;
	}

	if (taskWindow != nullptr) {
		delete taskWindow;
	}

	if (storage != nullptr) {
		delete storage;
	}

	if (spellObj != nullptr) {
		delete spellObj;
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
	
	taskWindow = new TaskWindow();
	inputWindow = new InputWindow();
	aboutWindow = new AboutWindow();
	systemTrayWidget = new SystemTrayWidget();
	hotKeyManager = new HotKeyManager();
	
	updateTaskWindow(storage->getTasks());
	showTaskWindow();
}

void Tasuke::setGuiMode(bool mode) {
	guiMode = mode;
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

void Tasuke::setStorage(IStorage* _storage) {
	LOG(INFO) << "Storage changed";

	storage = _storage;
}

InputWindow& Tasuke::getInputWindow(){
	return *inputWindow;
}

AboutWindow& Tasuke::getAboutWindow(){
	return *aboutWindow;
}

TaskWindow& Tasuke::getTaskWindow(){
	return *taskWindow;
}

// This function exposes the Storage instance for editing.
IStorage& Tasuke::getStorage() {
	return *storage;
}

void Tasuke::showInputWindow() {
	if (!guiMode) {
		return;
	}
	inputWindow->showAndCenter();
}

void Tasuke::showTaskWindow() {
	if (!guiMode) {
		return;
	}
	taskWindow->showAndMoveToSide();
}

void Tasuke::showAboutWindow(){
	if (!guiMode) {
		return;
	}
	aboutWindow->showAndCenter();
}

void Tasuke::hideInputWindow() {
	if (!guiMode) {
		return;
	}
	inputWindow->hide();
}

void Tasuke::hideTaskWindow() {
	if (!guiMode) {
		return;
	}
	taskWindow->hide();
}

void Tasuke::toggleInputWindow() {
	if (!guiMode) {
		return;
	}

	if (inputWindow->isVisible()) {
		hideInputWindow();
	} else {
		showInputWindow();
	}
}

void Tasuke::toggleTaskWindow() {
	if (!guiMode) {
		return;
	}

	if (taskWindow->isVisible()) {
		hideTaskWindow();
	} else {
		showTaskWindow();
	}
}

void Tasuke::showTutorial() {
	if (!guiMode) {
		return;
	}

	taskWindow->showTutorialWidget();
}

void Tasuke::showMessage(QString message) {
	LOG(INFO) << "Showing message: " << message.toStdString();

	if (!guiMode) {
		return;
	}

	systemTrayWidget->showMessage(message);
}

void Tasuke::updateTaskWindow(QList<Task> tasks) {
	if (!guiMode) {
		return;
	}

	LOG(INFO) << "Updating task window with " << QString::number(tasks.size()).toStdString() << " tasks";

	taskWindow->showTasks(tasks);
}

bool Tasuke::spellCheck(QString word) {
	if (word.size() == 0) {
		return true;
	} else if (!word[0].isLetter()) {
		return true;
	}

	return spellObj->spell(word.toUtf8().data());
}

// This function runs a command in a string
void Tasuke::runCommand(QString commandString) {
	try {
		QSharedPointer<ICommand> command = QSharedPointer<ICommand>(Interpreter::interpret(commandString));
		if (command == nullptr) {
			return;
		}
		command->run();

		LOG(INFO) << "Pushing command to history stack";
		commandUndoHistory.push_back(command);
		commandRedoHistory.clear();
	} catch (ExceptionBadCommand& exception) {
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
	QSharedPointer<ICommand> command = commandUndoHistory.back();
	commandUndoHistory.pop_back();
	command->undo();
	commandRedoHistory.push_back(command);
}

void Tasuke::redoCommand() {
	if (commandRedoHistory.size() == 0) {
		showMessage("Nothing to redo");
		return;
	}

	LOG(INFO) << "Redoing command";
	QSharedPointer<ICommand> command = commandRedoHistory.back();
	commandRedoHistory.pop_back();
	command->run();
	commandUndoHistory.push_back(command);
}