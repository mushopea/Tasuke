#include <string>
#include <iostream>
#include <thread>
#include <glog/logging.h>
#include <QMessageBox>
#include <QFontDatabase>
#include <QStandardPaths>
#include <QDir>
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

	qRegisterMetaType<TRY_RESULT>(METATYPE_TRY_RESULT);
	connect(this, SIGNAL(tryFinish(TRY_RESULT)), this, SLOT(handleTryFinish(TRY_RESULT)));
	connect(&inputTimer, SIGNAL(timeout()), this, SLOT(handleInputTimeout()));
	
	if (guiMode) {
		QTimer::singleShot(0, this, SLOT(initGui()));
	}
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

// loads the dictionary used by hunspell
// if the dictionaries cannot be found at the path, then spell checking 
// is disabled for the program
void Tasuke::loadDictionary() {
	LOG(INFO) << MSG_LOADING_DICTIONARY;

	spellCheckEnabled = true;

	if (!QFile(SPELL_GB_AFFFILE).exists() || !QFile(SPELL_GB_DICFILE).exists()) {
		spellCheckEnabled = false;
		return;
	}

	spellObj = new Hunspell(SPELL_GB_AFFFILE, SPELL_GB_DICFILE);
	spellObj->add_dic(SPELL_US_DICFILE);

	// adds additional words and non-words to dictionary
	foreach(QStringList list, SPELL_INCLUDE_LISTS) {
		foreach(QString word, list) {
			spellObj->add(word.toUtf8().data());
		}
	}
}

// loads all fonts in resources
void Tasuke::loadFonts(){
	LOG(INFO) << MSG_LOADING_FONTS;

	QFontDatabase fontDatabase; 
	foreach(QString font, FONT_LIST) {
		fontDatabase.addApplicationFont(font);
	}
}

// initialize the GUI if required
void Tasuke::initGui(){
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
	connect(settingsWindow, SIGNAL(themeChanged()), inputWindow, SLOT(handleReloadTheme()));
	connect(settingsWindow, SIGNAL(featuresChanged()), inputWindow, SLOT(handleReloadFeatures()));
	connect(settingsWindow, SIGNAL(iconsChanged()), inputWindow, SIGNAL(reloadIcons()));
	connect(settingsWindow, SIGNAL(fontChanged()), taskWindow, SLOT(displayTaskList()));
	connect(settingsWindow, SIGNAL(themeChanged()), taskWindow, SLOT(handleReloadTheme()));
	connect(settingsWindow, SIGNAL(themeChanged()), taskWindow, SIGNAL(themeChanged()));
}

// enables/disables gui mode
void Tasuke::setGuiMode(bool mode) {
	guiMode = mode;
}

// Static method that returns the sole instance of Tasuke.
Tasuke& Tasuke::instance() {
	static Tasuke *instance = 0;
	static bool alreadyCreated = false;

	if(instance == 0) {
		assert(alreadyCreated == false);
		alreadyCreated = true;

		instance = new Tasuke();
		return *instance;
	} else {
		return *instance;
	}
}

// changes the storage instance used by tasuke
void Tasuke::setStorage(IStorage* _storage) {
	assert(_storage != nullptr);;
	LOG(INFO) << MSG_STORAGE_CHANGED;

	storage = _storage;
}

// getter for inputwindow
InputWindow& Tasuke::getInputWindow(){
	assert(inputWindow != nullptr);

	return *inputWindow;
}

// getter for aboutWindow
AboutWindow& Tasuke::getAboutWindow(){
	assert(aboutWindow != nullptr);
	
	return *aboutWindow;
}

// getter for settingsWindow
SettingsWindow& Tasuke::getSettingsWindow(){
	assert(settingsWindow != nullptr);

	return *settingsWindow;
}

// getter for taskWindow
TaskWindow& Tasuke::getTaskWindow(){
	assert(taskWindow != nullptr);

	return *taskWindow;
}

// getter for hotKeyManager
HotKeyManager& Tasuke::getHotKeyManager() {
	assert(hotKeyManager != nullptr);

    return *hotKeyManager;
}

// This function exposes the Storage instance for editing.
IStorage& Tasuke::getStorage() {
	assert(storage != nullptr);

	return *storage;
}

// shows the input window. if gui is disabled does nothing.
void Tasuke::showInputWindow() {
	if (!guiMode) {
		return;
	}
	inputWindow->showAndCenter();
}

// shows the task window. if gui is disabled does nothing.
void Tasuke::showTaskWindow() {
	if (!guiMode) {
		return;
	}
	taskWindow->showAndMoveToSide();
}
 
// shows the about window. if gui is disabled does nothing.
void Tasuke::showAboutWindow(){
	if (!guiMode) {
		return;
	}
	aboutWindow->showAndCenter();
}

// hide the input window. if gui is disabled does nothing.
void Tasuke::hideInputWindow() {
	if (!guiMode) {
		return;
	}
	inputWindow->hide();
}

// hide the task window. if gui is disabled does nothing.
void Tasuke::hideTaskWindow() {
	if (!guiMode) {
		return;
	}
	taskWindow->hide();
}

// taggoles the input window. if gui is disabled does nothing.
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

// taggoles the task window. if gui is disabled does nothing.
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

// taggoles the input and task window. if gui is disabled does nothing.
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

// shows the tutorial. if gui is disabled does nothing.
void Tasuke::showTutorial() {
	if (!guiMode) {
		return;
	}

	if (!taskWindow->isVisible()) {
		showTaskWindow();
	}
	
	taskWindow->showTutorialWidget();
}

// shows the settings window. if gui is disabled does nothing.
void Tasuke::showSettingsWindow() {
	if (!guiMode) {
		return;
	}

	settingsWindow->showAndCenter();
}

// shows the messaage in the system tray
void Tasuke::showMessage(QString message) {
	LOG(INFO) << MSG_SHOWING_MESSAGE(message);

	if (!guiMode) {
		return;
	}

	systemTrayWidget->showMessage(message);
}

// Updates task windows with the latest task and tile.
// If not title is given, defaults to no title
void Tasuke::updateTaskWindow(QList<Task> tasks, QString title) {
	if (!guiMode) {
		return;
	}

	LOG(INFO) << MSG_UPDATING_TASKWINDOW(QString::number(tasks.size()));

	taskWindow->showTasks(tasks, title);
}

// Highlight the task with the given id in the task window
void Tasuke::highlightTask(int id) {
	if (!guiMode) {
		return;
	}

	LOG(INFO) << MSG_HIGHLIGHT_TASK(id);

	taskWindow->highlightTask(id);
}

// check if word is correctly spelt
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

bool Tasuke::setRunOnStartup(bool yes) {
#ifdef Q_OS_WIN
	QDir dir = QDir(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation));
	QString lnkPath = dir.absoluteFilePath(STARTUP_LNK_PATH);

	if (yes) {
		QString exePath = QCoreApplication::applicationFilePath();
		return QFile::link(exePath, lnkPath);
	} else {
		return QFile::remove(lnkPath);
	}
#else
	return false;
#endif
}

// formats the message displayed in tooltip feedback in rich text
QString Tasuke::formatTooltipMessage(QString commandString, QString errorString, QString errorWhere) {
	QString commandType = Interpreter::getType(commandString);

	// default display
	QString formatPart = FORMAT_ALL;
	QString descriptionPart = DESCRIPTION_ALL;

	// change based on detected command type
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

	// highlights the parts marked by pseudo tags
	if (!errorWhere.isEmpty()) {
		formatPart.replace(PSEUDO_TAG_BEGIN(errorWhere), HTML_ERROR_BEGIN);
		formatPart.replace(PSEUDO_TAG_END(errorWhere), HTML_ERROR_END);
	}

	// removes the pseudo tags used
	QRegExp braceRegex = BRACE_REGEX;
	braceRegex.setMinimal(true);
	formatPart = formatPart.remove(braceRegex);

	// replace description with error message if available
	if (!errorString.isEmpty()) {
		descriptionPart = errorString;
	}

	// formats the message using HTML
	QString message = QString(HTML_FORMAT_BEGIN + formatPart + HTML_FORMAT_END
		+ HTML_DESCRIPTION_BEGIN + descriptionPart + HTML_DESCRIPTION_END);

	// returns the message in rich text
	return message;
}

// This function runs a command in a string
void Tasuke::runCommand(QString commandString) {
	// if the validator is scheduled to run, stop it
	if (inputTimer.isActive()) {
		inputTimer.stop();
	}

	try {
		// interpret the command
		QSharedPointer<ICommand> command = QSharedPointer<ICommand>(Interpreter::interpret(commandString));

		// if gui enabled update ui
		if (guiMode) {
			inputWindow->hideTooltip();
			inputWindow->closeAndClear();
		}

		// if there is not command object end here
		if (command == nullptr) {
			return;
		}

		// actually run the object
		command->run();

		// put object into command history
		LOG(INFO) << MSG_COMMAND_STACK_PUSH;
		commandUndoHistory.push_back(command);
		commandRedoHistory.clear();

		// save the file after changes
		storage->saveFile();
	} catch (ExceptionBadCommand& exception) {
		// something went wrong, find out what and where
		QString errorString = exception.what();
		QString errorWhere = exception.where();
		LOG(INFO) << MSG_ERROR_PARSING(errorString);

		// if gui enabled, display the error in tooltip and shake the input box
		if (guiMode) {
			QString message = formatTooltipMessage(commandString, errorString, errorWhere);
			inputWindow->showTooltipMessage(InputStatus::FAILURE), message;
			inputWindow->doErrorAnimation();
		}
	}
}

// activates when user is typing in the command
void Tasuke::handleInputChanged(QString commandString) {
	input = commandString;
	
	// box is empty, hide it
	if (commandString.isEmpty()) {
		inputWindow->hideTooltip();
		return;
	}

	// display the message in tooltip
	QString message = formatTooltipMessage(commandString);
	inputWindow->showTooltipMessage(InputStatus::NORMAL, message);

	// if an evaluation is scheduled, unschedule it
	if (inputTimer.isActive()) {
		inputTimer.stop();
	}

	// schedule an evaluation
	inputTimer.setInterval(500);
	inputTimer.setSingleShot(true);
	inputTimer.start();
}

// activates when an evaluation is scheduled and triggered
void Tasuke::handleInputTimeout() {
	if (!mutex.tryLock()) {
		// try thread is still running! reschedule the timer
		inputTimer.setInterval(500);
		inputTimer.setSingleShot(true);
		inputTimer.start();
		return;
	}

	// box is empty, hide it
	if (input.isEmpty()) {
		inputWindow->hideTooltip();
		mutex.unlock();
		return;
	}

	// spawn another thread to evaluate command
	std::thread tryThread([&]() {
		try {
			// dry run interpret
			ICommand* command = Interpreter::interpret(input, true);

			// clean up if required
			if (command != nullptr) {
				delete command;
			}
		
			// capture results
			TRY_RESULT result;
			result.status = InputStatus::SUCCESS;
			emit tryFinish(result);
		} catch (ExceptionBadCommand& exception) {
			// something went wrong, find out what and where
			QString errorString = exception.what();
			QString errorWhere = exception.where();

			// capture the errors
			TRY_RESULT result;
			result.status = InputStatus::NORMAL;
			result.message = formatTooltipMessage(input, errorString, errorWhere);
			emit tryFinish(result);
		}
	});

	// let this thread run on its own
	tryThread.detach();
}

// activates when an evaluation has finished
void Tasuke::handleTryFinish(TRY_RESULT result) {
	if (inputWindow->isVisible() && !input.isEmpty()) {
		inputWindow->showTooltipMessage(result.status, result.message);
	}
	assert(mutex.tryLock() == false);
	mutex.unlock();
}

// undos the last command
// if there was no last command, nothing happens
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

// redos the last command
// if there was no command to redo, nothing happens
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

// returns the size of the undo history
int Tasuke::undoSize() const {
	return commandUndoHistory.size();
}

// returns the size of the redo history
int Tasuke::redoSize() const {
	return commandRedoHistory.size();
}