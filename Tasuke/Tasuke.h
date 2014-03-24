#ifndef TASUKE_H
#define TASUKE_H

#include <QSharedPointer>
#include "Commands.h"
#include "Storage.h"
#include "InputWindow.h"
#include "TaskWindow.h"
#include "AboutWindow.h"
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
	TaskWindow& getTaskWindow();

	void showInputWindow(); 
	void showTaskWindow();
	void showAboutWindow();
	void hideInputWindow();
	void hideTaskWindow();
	void toggleInputWindow();
	void toggleTaskWindow();
	void showMessage(QString message);
	void updateTaskWindow(QList<Task> tasks);

	void runCommand(QString commandString);
	void undoCommand();
	void redoCommand();

	void loadFonts();
	void initialize();
	
	static void setGuiMode(bool mode);
	static Tasuke &instance();

private:
	static bool guiMode;
	IStorage* storage;
	QList<QSharedPointer<ICommand>> commandUndoHistory;
	QList<QSharedPointer<ICommand>> commandRedoHistory;
	TaskWindow *taskWindow;
	InputWindow *inputWindow;
	AboutWindow *aboutWindow;
	SystemTrayWidget *systemTrayWidget;
	HotKeyManager *hotKeyManager;

	Tasuke();
	Tasuke(const Tasuke& old);
	const Tasuke& operator=(const Tasuke& old);
	~Tasuke();
};

#endif
