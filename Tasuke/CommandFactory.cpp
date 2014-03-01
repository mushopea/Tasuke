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
		for (int i=1; i<tokens.length(); i++) {
			description.append(tokens[i]);
			description.append(" ");
		}
		description.chop(1);
		task.setDescription(description);
		return std::shared_ptr<ICommand>(new AddCommand(task));
	} else if (commandType == "exit") {
		QApplication::quit();
		return nullptr;
	} else if (commandType == "undo") {
		Tasuke::instance().undoCommand();
		return nullptr;
	} else {
		throw ExceptionBadCommand();
	}
}