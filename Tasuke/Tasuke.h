#ifndef TASUKE_H
#define TASUKE_H

#include <hunspell/hunspell.hxx>
#include <QSharedPointer>
#include "Commands.h"
#include "Storage.h"
#include "InputWindow.h"
#include "TaskWindow.h"
#include "AboutWindow.h"
#include "SettingsWindow.h"
#include "SystemTrayWidget.h"
#include "HotKeyManager.h"

// This class handles the control flow of the entire program. This class is a
// singleton; it cannot be created anywhere else because its constructor and
// destructor is private. The only way to retrieve an instance of this
// singleton is the instance() method which guarantees there is only one sole
// instance of this class.
class Tasuke {
public:
	void setStorage(IStorage* _storage);
	IStorage& getStorage();
	InputWindow& getInputWindow();
	AboutWindow& getAboutWindow();
	SettingsWindow& getSettingsWindow();
	TaskWindow& getTaskWindow();
    HotKeyManager& getHotKeyManager();

	void showInputWindow();
	void showTaskWindow();
	void showAboutWindow();
	void hideInputWindow();
	void hideTaskWindow();
	void toggleInputWindow();
	void toggleTaskWindow();
	void showSettingsWindow();
	void showTutorial();
	void showMessage(QString message);
	void updateTaskWindow(QList<Task> tasks);
	bool spellCheck(QString word);

	void runCommand(QString commandString);
	void undoCommand();
	void redoCommand();

	void loadDictionary();
	void loadFonts();
	void initialize();

	static void setGuiMode(bool mode);
	static Tasuke &instance();

private:
	static bool guiMode;
	IStorage* storage;
	QList<QSharedPointer<ICommand> > commandUndoHistory;
	QList<QSharedPointer<ICommand> > commandRedoHistory;
	TaskWindow* taskWindow;
	InputWindow* inputWindow;
	AboutWindow* aboutWindow;
	SettingsWindow* settingsWindow;
	SystemTrayWidget* systemTrayWidget;
	HotKeyManager* hotKeyManager;
	Hunspell* spellObj;

	Tasuke();
	Tasuke(const Tasuke& old);
	const Tasuke& operator=(const Tasuke& old);
	~Tasuke();
};

#endif
