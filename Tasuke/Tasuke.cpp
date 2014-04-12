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
	connect(settingsWindow, SIGNAL(themeChanged()), inputWindow, SLOT(reloadTheme()));
	connect(settingsWindow, SIGNAL(featuresChanged()), inputWindow, SLOT(reloadFeatures()));
	connect(settingsWindow, SIGNAL(iconsChanged()), inputWindow, SIGNAL(reloadIcons()));
	connect(settingsWindow, SIGNAL(fontChanged()), taskWindow, SLOT(displayTaskList()));
	connect(settingsWindow, SIGNAL(themeChanged()), taskWindow, SLOT(reloadTheme()));
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

	QString formatPart = FORMAT_ALL;
	QString descriptionPart = DESCRIPTION_ALL;

	if (commandType == COMMAND_ADD) {
		if (commandString.contains(ADD_PREIOD_REGEX)) {
			// period tasks
			formatPart = FORMAT_ADD_PERIOD;
			descriptionPart = DESCRIPTION_ADD_PERIOD;
		} else if (commandString.contains(ADD_DEADLINE_REGEX)) {
			// deadline tasks
			formatPart = FORMAT_ADD_DEADLINE;
			descriptionPart = DESCRIPTION_ADD_DEADLINE;
		} else {
			// simple tasks
			formatPart = FORMAT_ADD;
			descriptionPart = DESCRIPTION_ADD;
		}
	} else if (commandType == COMMAND_REMOVE) {
		formatPart = FORMAT_REMOVE;
		descriptionPart = DESCRIPTION_REMOVE;
	} else if (commandType == COMMAND_EDIT) {
		formatPart = FORMAT_EDIT;
		descriptionPart = DESCRIPTION_EDIT;
	} else if (commandType == COMMAND_DONE) {
		formatPart = FORMAT_DONE;
		descriptionPart = DESCRIPTION_DONE;
	} else if (commandType == COMMAND_UNDONE) {
		formatPart = FORMAT_UNDONE;
		descriptionPart = DESCRIPTION_UNDONE;
	} else if (commandType == COMMAND_SHOW) {
		formatPart = FORMAT_SHOW;
		descriptionPart = DESCRIPTION_SHOW;
	} else if (commandType == COMMAND_HIDE) {
		formatPart = FORMAT_HIDE;
		descriptionPart = DESCRIPTION_HIDE;
	} else if (commandType == COMMAND_UNDO) {
		formatPart = FORMAT_UNDO;
		descriptionPart = DESCRIPTION_UNDO;
	} else if (commandType == COMMAND_REDO) {
		formatPart = FORMAT_REDO;
		descriptionPart = DESCRIPTION_REDO;
	} else if (commandType == COMMAND_CLEAR) {
		formatPart = FORMAT_CLEAR;
		descriptionPart = DESCRIPTION_CLEAR;
	} else if (commandType == COMMAND_HELP) {
		formatPart = FORMAT_HELP;
		descriptionPart = DESCRIPTION_HELP;
	} else if (commandType == COMMAND_SETTINGS) {
		formatPart = FORMAT_SETTINGS;
		descriptionPart = DESCRIPTION_SETTINGS;
	} else if (commandType == COMMAND_ABOUT) {
		formatPart = FORMAT_ABOUT;
		descriptionPart = DESCRIPTION_ABOUT;
	} else if (commandType == COMMAND_EXIT) {
		formatPart = FORMAT_EXIT;
		descriptionPart = DESCRIPTION_EXIT;
	}

	if (!errorWhere.isEmpty()) {
		formatPart.replace(PSEUDO_TAG_BEGIN(errorWhere), HTML_ERROR_BEGIN);
		formatPart.replace(PSEUDO_TAG_END(errorWhere), HTML_ERROR_END);
	}

	QRegExp braceRegex = BRACE_REGEX;
	braceRegex.setMinimal(true);
	formatPart = formatPart.remove(braceRegex);

	if (!errorString.isEmpty()) {
		descriptionPart = errorString;
	}

	QString message = QString(HTML_FORMAT_BEGIN + formatPart + HTML_FORMAT_END
		+ HTML_DESCRIPTION_BEGIN + descriptionPart + HTML_DESCRIPTION_END);


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