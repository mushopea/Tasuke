#include <QApplication>
#include <QStringList>
#include "Tasuke.h"
#include "Commands.h"
#include "Constants.h"
#include "Exceptions.h"
#include "CommandFactory.h"

// This static helper function returns an instance of a ICommand that represents
// the user's command. The caller must clean up using delete.
std::shared_ptr<ICommand> CommandFactory::interpret(const std::string& command) {
	QString commandString(command.c_str());
	QStringList tokens = commandString.trimmed().split(" ");
	QString commandType = tokens[0];

	if (commandType == "add") {
		Task task;
		QString description;
		for (int i=1; i<tokens.size(); i++) {
			description.append(tokens[i]);
			description.append(" ");
		}
		description.chop(1);
		task.setDescription(description);
		return std::shared_ptr<ICommand>(new AddCommand(task));
	} else if (commandType == "remove") {
		if (tokens.size() != 2) {
			throw ExceptionBadCommand();
		}
		int pos = tokens[1].toInt();
		int maxPos = Tasuke::instance().getStorage().totalTasks();

		if (pos < 1 || pos > maxPos) {
			throw ExceptionBadCommand();
		}
		return std::shared_ptr<ICommand>(new RemoveCommand(pos-1));
	} else if (commandString == "show") {
		Tasuke::instance().showTaskWindow();
		return nullptr;
	} else if (commandString == "hide") {
		Tasuke::instance().hideTaskWindow();
		return nullptr;
	} else if (commandString == "undo") {
		Tasuke::instance().undoCommand();
		return nullptr;
	} else if (commandString == "exit") {
		QApplication::quit();
		return nullptr;
	} else {
		throw ExceptionBadCommand();
	}
}