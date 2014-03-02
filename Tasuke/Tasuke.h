#ifndef TASUKE_H
#define TASUKE_H

#include <memory>
#include <vector>
#include "Commands.h"
#include "Storage.h"
#include "InputWindow.h"
#include "TaskWindow.h"

// This class handles the control flow of the entire program. This class is a
// singleton; it cannot be created anywhere else because its constructor and
// destructor is private. The only way to retrieve an instance of this
// singleton is the instance() method which guarantees there is only one sole
// instance of this class.
class Tasuke {
private:
	Storage* storage;
	std::vector<std::shared_ptr<ICommand>> commandUndoHistory;
	std::vector<std::shared_ptr<ICommand>> commandRedoHistory;
	InputWindow inputWindow;
	TaskWindow taskWindow;

	Tasuke();
	Tasuke(const Tasuke& old);
	const Tasuke& operator=(const Tasuke& old);
	~Tasuke();
public:
	void setStorage(Storage* _storage);
	Storage& getStorage();

	void showInputWindow();
	void showTaskWindow();
	void hideTaskWindow();
	void showMessage(QString message);
	void updateTaskWindow(QList<Task> tasks);
	
	void runCommand(std::string commandString);
	void undoCommand();
	void redoCommand();

	static Tasuke &instance();
};

#endif
