//@author A0096836M

#ifndef TASUKE_H
#define TASUKE_H

#include <future>
#include <thread>
#include <hunspell/hunspell.hxx>
#include <QObject>
#include <QTimer>
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
class Tasuke : public QObject {
	Q_OBJECT

public:
	typedef struct {
		QString message;
		InputStatus status;
	} TRY_RESULT;

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
	void toggleBothWindows();
	void showSettingsWindow();
	void showTutorial();
	void showMessage(QString message);
	void updateTaskWindow(QList<Task> tasks, QString title = "");
	void highlightTask(int id);
	bool spellCheck(QString word);
	bool setRunOnStartup(bool yes);
	QString formatTooltipMessage(QString commandString, 
		QString errorString = "", QString errorWhere = "");

	void runCommand(QString commandString);
	void undoCommand();
	void redoCommand();
	int undoSize() const;
	int redoSize() const;
	void limitUndoRedo();

	void loadDictionary();
	void loadFonts();

	static void setGuiMode(bool mode);
	static Tasuke &instance();

signals:
	void tryFinish(TRY_RESULT result);

private slots:
	void initGui();
	void handleInputChanged(QString text);
	void handleInputTimeout();
	void handleTryFinish(TRY_RESULT result);

private:
	static bool guiMode;
	IStorage* storage;
	QList< QSharedPointer<ICommand> > commandUndoHistory;
	QList< QSharedPointer<ICommand> > commandRedoHistory;
	TaskWindow* taskWindow;
	InputWindow* inputWindow;
	AboutWindow* aboutWindow;
	SettingsWindow* settingsWindow;
	SystemTrayWidget* systemTrayWidget;
	HotKeyManager* hotKeyManager;
	Hunspell* spellObj;
	QMutex mutex;
	QTimer inputTimer;
	QString input;
	bool spellCheckEnabled;

	Tasuke();
	Tasuke(const Tasuke& old);
	const Tasuke& operator=(const Tasuke& old);
	~Tasuke();
};

#endif
