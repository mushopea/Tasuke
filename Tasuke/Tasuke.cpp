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

//@author A0096836M

bool Tasuke::guiMode = true;

// Constructor for the Tasuke singleton.
Tasuke::Tasuke() : QObject(nullptr) {
	LOG(INFO) << MSG_TASUKE_CREATED;
	
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

	qRegisterMetaType<TRY_RESULT>(METATYPE_TRY_RESULT);
	connect(this, SIGNAL(tryFinish(TRY_RESULT)), this, SLOT(handleTryFinish(TRY_RESULT)));

	connect(&inputTimer, SIGNAL(timeout()), this, SLOT(handleInputTimeout()));
}

// Destructor for the Tasuke singleton.
Tasuke::~Tasuke() {
	LOG(INFO) << MSG_TASUKE_DESTROYED;

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
	LOG(INFO) << MSG_LOADING_DICTIONARY;

	spellCheckEnabled = true;

	if (!QFile(SPELL_GB_AFFFILE).exists() || !QFile(SPELL_GB_DICFILE).exists()) {
		spellCheckEnabled = false;
		return;
	}

#ifndef Q_OS_MAC
	spellObj = new Hunspell(SPELL_GB_AFFFILE, SPELL_GB_DICFILE);
	spellObj->add_dic(SPELL_US_DICFILE);
#else
	QString path = QCoreApplication::applicationDirPath();
	spellObj = new Hunspell((path + MAC_RESOURCE_PATH + SPELL_GB_AFFFILE).toUtf8().constData(), (path + MAC_RESOURCE_PATH + SPELL_GB_DICFILE).toUtf8().constData());
	spellObj->add_dic((path + MAC_RESOURCE_PATH + SPELL_US_DICFILE).toUtf8().constData());
#endif

	foreach(QStringList list, SPELL_INCLUDE_LISTS) {
		foreach(QString word, list) {
			spellObj->add(word.toUtf8().data());
		}
	}
}

void Tasuke::loadFonts(){
	LOG(INFO) << MSG_LOADING_FONTS;

	QFontDatabase fontDatabase; 
	foreach(QString font, FONT_LIST) {
		fontDatabase.addApplicationFont(font);
	}
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
	LOG(INFO) << MSG_STORAGE_CHANGED;

	storage = _storage;
}

InputWindow& Tasuke::getInputWindow(){
	assert(inputWindow != nullptr);

	return *inputWindow;
}

AboutWindow& Tasuke::getAboutWindow(){
	assert(aboutWindow != nullptr);
	
	return *aboutWindow;
}

SettingsWindow& Tasuke::getSettingsWindow(){
	assert(settingsWindow != nullptr);

	return *settingsWindow;
}

TaskWindow& Tasuke::getTaskWindow(){
	assert(taskWindow != nullptr);

	return *taskWindow;
}

HotKeyManager& Tasuke::getHotKeyManager() {
	assert(hotKeyManager != nullptr);

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
	LOG(INFO) << MSG_SHOWING_MESSAGE(message);

	if (!guiMode) {
		return;
	}

	systemTrayWidget->showMessage(message);
}

void Tasuke::updateTaskWindow(QList<Task> tasks, QString title) {
	if (!guiMode) {
		return;
	}

	LOG(INFO) << MSG_UPDATING_TASKWINDOW(QString::number(tasks.size()));

	taskWindow->showTasks(tasks, title);
}

void Tasuke::highlightTask(int id) {
	if (!guiMode) {
		return;
	}

	LOG(INFO) << MSG_HIGHLIGHT_TASK(id);

	taskWindow->highlightTask(id);
}

bool Tasuke::spellCheck(QString word) {
	if (!spellCheckEnabled) {
		return true;
	}

	if (word.size() == 0) {
		return true;
	} else if (!word[0].isLetter()) {
		return true;
	} else if (word[0].isUpper()) {
		return true;
	}

	if (spellObj->spell(word.toUtf8().data())) {
		return true;
	}

	return false;
}

QString Tasuke::formatTooltipMessage(QString commandString, QString errorString, QString errorWhere) {
	QString commandType = Interpreter::getType(commandString);

	QString formatPart = "add | edit | done | undone | remove | show | undo | redo | settings | help | exit";
	QString descriptionPart = "Use one of these keywords to begin";

	if (commandType == "add") {
		if (commandString.contains(QRegExp("\\bfrom\\b"))) { // period tasks
			formatPart = "add {description}[my task]{/description} from {date}{start}[start]{/star} to {end}[end]{/end}{/date} {tag}#tag{/tag}";
			descriptionPart = "Adds a task with a time period.";
		} else if (commandString.contains(QRegExp("\\b(by|at|on)\\b"))) { // deadline tasks
			formatPart = "add {description}[my task]{/description} by/on/at {date}{end}[end]{/end}{/date} {tag}#tag{/tag}";
			descriptionPart = "Adds a task with a deadline.";
		} else { // simple tasks
			formatPart = "add {description}[my task]{/description} {tag}#tag{/tag}";
			descriptionPart = "Adds a simple task.";
		}
	} else if (commandType == "remove") {
		formatPart = "remove {id}[task no]{/id} | remove [task no], [task no], ... | remove [task no] - [task no]";
		descriptionPart = "Removes task(s) from the list.";
	} else if (commandType == "edit") {
		formatPart = "edit {id}[task no]{/id} {description}[thing to change] -[thing to remove]{/description}";
		descriptionPart = "Edits existing task.";
	} else if (commandType == "done") {
		formatPart = "done {id}[task no]{/id} | done [task no], [task no], ... | done [task no] - [task no]";
		descriptionPart = "Marks tasks as done.";
	} else if (commandType == "undone") {
		formatPart = "undone {id}[task no]{/id} | undone [task no], [task no], ... | undone [task no] - [task no]";
		descriptionPart = "Marks tasks as undone.";
	} else if (commandType == "show") {
		formatPart = "show [keyword] | done | undone | overdue | ongoing | today | tomorrow";
		descriptionPart = "Shows certain tasks.";
	} else if (commandType == "hide") {
		formatPart = "hide";		
		descriptionPart = "Hides the task list.";
	} else if (commandType == "undo") {
		formatPart = "undo {times}[times]{/times} | max";	
		descriptionPart = "Undos your last command(s).";
	} else if (commandType == "redo") {
		formatPart = "redo {times}[times]{/times} | max";
		descriptionPart = "Redos your last command(s).";
	} else if (commandType == "clear") {
		formatPart = "clear";
		descriptionPart = "Clears all tasks in your list.";
	} else if (commandType == "help") {
		formatPart = "help";
		descriptionPart = "Shows the tutorial.";
	} else if (commandType == "settings") {
		formatPart = "settings";
		descriptionPart = "Open the settings window.";
	} else if (commandType == "about") {
		formatPart = "about";
		descriptionPart = "Shows about Tasuke.";
	} else if (commandType == "exit") {
		formatPart = "exit";
		descriptionPart = "Exits the program.";
	}

	if (!errorWhere.isEmpty()) {
		formatPart = formatPart.replace("{"+errorWhere+"}", "<font color='#FA7597'>");
		formatPart = formatPart.replace("{/"+errorWhere+"}", "</font>");
	}

	QRegExp braceRegex = QRegExp("\\{(.*)\\}");
	braceRegex.setMinimal(true);
	formatPart = formatPart.remove(braceRegex);

	if (!errorString.isEmpty()) {
		descriptionPart = errorString;
	}

	QString message = QString("<font color='#999'>" + formatPart+ "</font>"
		"<br<<font color='white'>"+descriptionPart + "</font>");


	return message;
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

		LOG(INFO) << MSG_COMMAND_STACK_PUSH;
		commandUndoHistory.push_back(command);
		commandRedoHistory.clear();

		storage->saveFile();
	} catch (ExceptionBadCommand& exception) {
		QString errorString = exception.what();
		QString errorWhere = exception.where();
		LOG(INFO) << MSG_ERROR_PARSING(errorString);

		if (guiMode) {
			QString message = formatTooltipMessage(commandString, errorString, errorWhere);
			inputWindow->showTooltipMessage(InputStatus::FAILURE), message;
			inputWindow->doErrorAnimation();
		}
	}
}

void Tasuke::handleInputChanged(QString commandString) {
	input = commandString;
	
	if (commandString.isEmpty()) {
		inputWindow->hideTooltip();
		return;
	}

	QString message = formatTooltipMessage(commandString);

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

	if (input.isEmpty()) {
		inputWindow->hideTooltip();
		mutex.unlock();
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
			QString errorString = exception.what();
			QString errorWhere = exception.where();
			TRY_RESULT result;
			result.status = InputStatus::NORMAL;
			result.message = formatTooltipMessage(input, errorString, errorWhere);
			emit tryFinish(result);
		}
	});
	tryThread.detach();
}

void Tasuke::handleTryFinish(TRY_RESULT result) {
	if (inputWindow->isVisible() && !input.isEmpty()) {
		inputWindow->showTooltipMessage(result.status, result.message);
	}
	mutex.unlock();
}

void Tasuke::undoCommand() {
	if (commandUndoHistory.size() == 0) {
		showMessage(MSG_NO_UNDO);
		return;
	}

	LOG(INFO) << MSG_UNDO;
	QSharedPointer<ICommand> command = commandUndoHistory.back();
	commandUndoHistory.pop_back();
	command->undo();
	commandRedoHistory.push_back(command);
	storage->saveFile();
}

void Tasuke::redoCommand() {
	if (commandRedoHistory.size() == 0) {
		showMessage(MSG_NO_REDO);
		return;
	}

	LOG(INFO) << MSG_REDO;
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