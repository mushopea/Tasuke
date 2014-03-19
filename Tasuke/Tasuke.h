#ifndef TASUKE_H
#define TASUKE_H

#include <memory>
#include <vector>
#include "Commands.h"
#include "Storage.h"
#include "InputWindow.h"
#include "TaskWindow.h"
#include "AboutWindow.h"


// This class handles the control flow of the entire program. This class is a
// singleton; it cannot be created anywhere else because its constructor and
// destructor is private. The only way to retrieve an instance of this
// singleton is the instance() method which guarantees there is only one sole
// instance of this class.
class Tasuke : public QWidget {
	Q_OBJECT

public:
	void setStorage(Storage* _storage);
	Storage& getStorage();
	InputWindow& getInputWindow();
	AboutWindow& getAboutWindow();
	TaskWindow& getTaskWindow();

	void loadFonts();
	void initialize();
	
	static Tasuke &instance();

public slots:
	void showInputWindow(); 
	void showTaskWindow();
	void showAboutWindow();
	void hideTaskWindow();
	void showMessage(QString message);
	void updateTaskWindow(QList<Task> tasks);

	void runCommand(QString commandString);
	void undoCommand();
	void redoCommand();

private slots:
	void handleHotKeyPress(int key);
	void handleIconActivated(QSystemTrayIcon::ActivationReason reason);
	
private:
	Storage* storage;
	std::vector<std::shared_ptr<ICommand>> commandUndoHistory;
	std::vector<std::shared_ptr<ICommand>> commandRedoHistory;
	TaskWindow taskWindow;
	InputWindow inputWindow;
	AboutWindow aboutWindow;
	HotKeyThread *hotKeyThread;
	QSystemTrayIcon* trayIcon;

	Tasuke(QWidget* parent = 0);
	Tasuke(const Tasuke& old);
	const Tasuke& operator=(const Tasuke& old);
	~Tasuke();

	void contextMenuOperations();
};

#endif
