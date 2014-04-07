#include <string>
#include <iostream>
#include <thread>
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
Tasuke::Tasuke() : QObject(nullptr) {
	LOG(INFO) << "Tasuke object created";
	
	loadDictionary();
	
	storage = new Storage();
	storage->loadFile();

	taskWindow = nullptr;
	inputWindow = nullptr;
	aboutWindow = nullptr;
	settingsWindow = nullptr;
	systemTrayWidget = nullptr;
	hotKeyManager = nullptr;

	std::thread dateFormatGeneratorThread([]() {
		Interpreter::initFormats();
	});

	dateFormatGeneratorThread.detach();

	if (guiMode) {
		initialize();
	}

	qRegisterMetaType<TRY_RESULT>("TRY_RESULT");
	connect(this, SIGNAL(tryFinish(TRY_RESULT)), this, SLOT(handleTryFinish(TRY_RESULT)));

	connect(&inputTimer, SIGNAL(timeout()), this, SLOT(handleInputTimeout()));
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
	
	if (settingsWindow != nullptr) {
		delete settingsWindow;
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

void Tasuke::loadDictionary() {
#ifndef Q_OS_MAC
	spellObj = new Hunspell("en_GB.aff", "en_GB.dic");
	spellObj->add_dic("en_US.dic");
#else
	QString path = QCoreApplication::applicationDirPath();
	spellObj = new Hunspell((path + "/../Resources/en_GB.aff").toUtf8().constData(), (path + "/../Resources/en_GB.dic").toUtf8().constData());
	spellObj->add_dic((path + "/../Resources/en_US.dic").toUtf8().constData());
#endif

	// commands
	spellObj->add("rm");
	spellObj->add("ls");
	spellObj->add("nd");

	// months
	spellObj->add("jan");
	spellObj->add("feb");
	spellObj->add("mar");
	spellObj->add("apr");
	spellObj->add("may");
	spellObj->add("jun");
	spellObj->add("jul");
	spellObj->add("aug");
	spellObj->add("sep");
	spellObj->add("oct");
	spellObj->add("nov");
	spellObj->add("dec");

	// days
	spellObj->add("mon");
	spellObj->add("tue");
	spellObj->add("wed");
	spellObj->add("thu");
	spellObj->add("fri");
	spellObj->add("sat");
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
	settingsWindow = new SettingsWindow();
	systemTrayWidget = new SystemTrayWidget();
	hotKeyManager = new HotKeyManager();
	
	updateTaskWindow(storage->getTasks());
	showTaskWindow();

	connect(inputWindow, SIGNAL(inputChanged(QString)), this, SLOT(handleInputChanged(QString)));
}

void Tasuke::setGuiMode(bool mode) {
	guiMode = mode;
}

// Static method that returns the sole instance of Tasuke.
Tasuke& Tasuke::instance() {
	static Tasuke *instance = 0;

	if(instance == 0) {
		// Allocates memory *before* constructor, so Tasuke::instance() will work within a constructor-called method
		instance = (Tasuke *) ::operator new (sizeof(Tasuke));
		// Actually runs the constructor now
		new (instance) Tasuke;
		//instance = new Tasuke();
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

SettingsWindow& Tasuke::getSettingsWindow(){
	return *settingsWindow;
}

TaskWindow& Tasuke::getTaskWindow(){
	return *taskWindow;
}

HotKeyManager& Tasuke::getHotKeyManager() {
    return *hotKeyManager;
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

void Tasuke::toggleBothWindows() {
	if (!guiMode) {
		return;
	}

	// If one of them is visible, hide all.
	if (taskWindow->isVisible() || inputWindow->isVisible()) {
		hideTaskWindow();
		hideInputWindow();
	} else { // None are visible, show both.
		showTaskWindow();
		showInputWindow();
	}
}

void Tasuke::showTutorial() {
	if (!guiMode) {
		return;
	}

	if (!taskWindow->isVisible()) {
		showTaskWindow();
	}
	
	taskWindow->showTutorialWidget();
}

void Tasuke::showSettingsWindow() {
	if (!guiMode) {
		return;
	}

	settingsWindow->showAndCenter();
}

void Tasuke::showMessage(QString message) {
	LOG(INFO) << "Showing message: " << message.toStdString();

	if (!guiMode) {
		return;
	}

	systemTrayWidget->showMessage(message);
}

void Tasuke::updateTaskWindow(QList<Task> tasks, QString title) {
	if (!guiMode) {
		return;
	}

	LOG(INFO) << "Updating task window with " << QString::number(tasks.size()).toStdString() << " tasks";

	taskWindow->showTasks(tasks, title);
}

void Tasuke::highlightTask(int id) {
	if (!guiMode) {
		return;
	}

	LOG(INFO) << "Highlighting task with id  " << id;

	taskWindow->highlightTask(id);
}

bool Tasuke::spellCheck(QString word) {
	if (word.size() == 0) {
		return true;
	} else if (!word[0].isLetter()) {
		return true;
	}

	if (spellObj->spell(word.toUtf8().data())) {
		return true;
	}

	return false;
}

// This function runs a command in a string
void Tasuke::runCommand(QString commandString) {
	if (inputTimer.isActive()) {
		inputTimer.stop();
	}

	try {
		QSharedPointer<ICommand> command = QSharedPointer<ICommand>(Interpreter::interpret(commandString));

		if (guiMode) {
			inputWindow->hideTooltip();
			inputWindow->closeAndClear();
		}

		if (command == nullptr) {
			return;
		}
		command->run();

		LOG(INFO) << "Pushing command to history stack";
		commandUndoHistory.push_back(command);
		commandRedoHistory.clear();

		storage->saveFile();
	} catch (ExceptionBadCommand& exception) {
		LOG(INFO) << "Error parsing command";
		
		showMessage("Error parsing command");

		if (guiMode) {
			inputWindow->showTooltipMessage(InputStatus::FAILURE);
			inputWindow->doErrorAnimation();
		}
	}
}

void Tasuke::handleInputChanged(QString commandString) {
	if (commandString.isEmpty()) {
		inputWindow->hideTooltip();
		return;
	}

	input = commandString;
	QString commandType = Interpreter::getType(commandString);

	QString message = "...";

	// TODO!!
	// TODO!! make this cleaner
	// TODO!!

	if (commandType == "add") {
		if (commandString.contains(QRegExp("\\bfrom\\b"))) { // period tasks
			message = "add <my task> from <start> to <end> #tag";
		} else if (commandString.contains(QRegExp("\\b(by|at|on)\\b"))) { // deadline tasks
			message = "add <my task> by/on/at <end> #tag";
		} else { // simple tasks
			message = "add <my task> #tag";
		}
	} else if (commandType == "remove") {
		message = "remove <task no> | remove <task no>, <task no>, ... | remove <task no> - <task no>";
	} else if (commandType == "edit") {
		message = "edit <task no> <thing to change> <-thing to remove>";
	} else if (commandType == "done") {
		message = "done <task no> | done <task no>, <task no>, ... | done <task no> - <task no>";
	} else if (commandType == "undone") {
		message = "undone <task no> | undone <task no>, <task no>, ... | undone <task no> - <task no>";
	} else if (commandType == "show") {
		message = "show <keyword> | done | undone | overdue | ongoing | today | tomorrow";
	} else if (commandType == "hide") {
		message = "Hide the task window.";		
	} else if (commandType == "undo") {
		message = "Undo your last action. (CTRL+Z)";		
	} else if (commandType == "redo") {
		message = "Redo your last action (CTRL+Y)";		
	} else if (commandType == "clear") {
		message = "Clear all tasks";
	} else if (commandType == "help") {
		message = "View the tutorial";
	} else if (commandType == "settings") {
		message = "Access the settings";
	} else if (commandType == "about") {
		message = "See Tasuke's info";
	} else if (commandType == "exit") {
		message = "Exit the application";
	}

	inputWindow->showTooltipMessage(InputStatus::NORMAL, message);

	if (inputTimer.isActive()) {
		inputTimer.stop();
	}

	inputTimer.setInterval(500);
	inputTimer.setSingleShot(true);
	inputTimer.start();
}

void Tasuke::handleInputTimeout() {
	if (!mutex.tryLock()) {
		// try thread is still running! reschedule the timer
		inputTimer.setInterval(500);
		inputTimer.setSingleShot(true);
		inputTimer.start();

		return;
	}

	std::thread tryThread([&]() {
		try {
			ICommand* command = Interpreter::interpret(input, true);
			if (command != nullptr) {
				delete command;
			}
		
			TRY_RESULT result;
			result.status = InputStatus::SUCCESS;
			emit tryFinish(result);
		} catch (ExceptionBadCommand& exception) {
			TRY_RESULT result;
			result.status = InputStatus::NORMAL;
			emit tryFinish(result);
		}
	});
	tryThread.detach();
}

void Tasuke::handleTryFinish(TRY_RESULT result) {
	if (inputWindow->isVisible()) {
		inputWindow->showTooltipMessage(result.status, result.message);
	}
	mutex.unlock();
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
	storage->saveFile();
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
	storage->saveFile();
}

int Tasuke::undoSize() const {
	return commandUndoHistory.size();
}

int Tasuke::redoSize() const {
	return commandRedoHistory.size();
}