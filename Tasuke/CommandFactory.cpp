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
			if (tokens[i][0] == '@' || tokens[i] == "-" || tokens[i][0] == '#') {
				break;
			}

			description.append(tokens[i]);
			description.append(" ");
		}
		description.chop(1);

		int atPos = commandString.indexOf("@");
		if (atPos > 0) {
			QString atString = commandString.remove(0,atPos+1);
			if (atString.contains("@")) {
				throw ExceptionBadCommand();
			}
			
			int tagPos = atString.indexOf("#");
			if (tagPos > 0) {
				atString.truncate(tagPos);
			}

			if (atString.trimmed().isEmpty()) {
				throw ExceptionBadCommand();
			}

			QStringList dateTimePeriod = atString.split("-");

			if (dateTimePeriod.size() == 1) {
				QDateTime end = parseDate(dateTimePeriod[0]);
				task.setEnd(end);

				if (!end.isValid()) {
					throw ExceptionBadCommand();
				}
			} else if (dateTimePeriod.size() == 2) {
				QDateTime begin = parseDate(dateTimePeriod[0]);
				QDateTime end = parseDate(dateTimePeriod[1]);
				task.setBegin(begin);
				task.setEnd(end);

				if (!end.isValid() || !begin.isValid()) {
					throw ExceptionBadCommand();
				}
			} else {
				throw ExceptionBadCommand();
			}
		}

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
	} else if (commandType == "edit") {
		if (tokens.size() < 2) {
			throw ExceptionBadCommand();
		}
		int pos = tokens[1].toInt();
		int maxPos = Tasuke::instance().getStorage().totalTasks();

		if (pos < 1 || pos > maxPos) {
			throw ExceptionBadCommand();
		}

		Task task = Tasuke::instance().getStorage().getTask(pos-1);;

		QString description;
		for (int i=2; i<tokens.size(); i++) {
			description.append(tokens[i]);
			description.append(" ");
		}
		description.chop(1);
		task.setDescription(description);
		return std::shared_ptr<ICommand>(new EditCommand(pos-1, task));
	} else if (commandString == "show") {
		Tasuke::instance().showTaskWindow();
		return nullptr;
	} else if (commandString == "about") {
		Tasuke::instance().showAboutWindow();
		return nullptr;
	} else if (commandString == "hide") {
		Tasuke::instance().hideTaskWindow();
		return nullptr;
	} else if (commandString == "undo") {
		Tasuke::instance().undoCommand();
		return nullptr;
	} else if (commandString == "redo") {
		Tasuke::instance().redoCommand();
		return nullptr;
	} else if (commandString == "exit") {
		QApplication::quit();
		return nullptr;
	} else {
		throw ExceptionBadCommand();
	}
}

QDateTime CommandFactory::parseDate(QString dateString) {
	dateString = dateString.trimmed();
	QDateTime retVal;
	QStringList fullDateFormats;
	QStringList dateOnlyFormats;
	QStringList timeOnlyFormats;

	fullDateFormats << "d/M/yy h.mm ap";
	fullDateFormats << "d/M/yy h.mm";
	fullDateFormats << "dd/MM/yy h.mm ap";
	fullDateFormats << "dd/MM/yy h.mm";
	fullDateFormats << "dd/MM/yy hh.mm ap";
	fullDateFormats << "dd/MM/yy hh.mm";
	fullDateFormats << "d/M h.mm ap";
	fullDateFormats << "d/M h.mm";
	fullDateFormats << "dd/MM h.mm ap";
	fullDateFormats << "dd/MM h.mm";
	fullDateFormats << "dd/MM hh.mm ap";
	fullDateFormats << "dd/MM hh.mm";
	fullDateFormats << "d MMM yy h.mm ap";
	fullDateFormats << "d MMM yy h.mm";
	fullDateFormats << "dd MMM yy h.mm ap";
	fullDateFormats << "dd MMM yy h.mm";
	fullDateFormats << "dd MMM yy hh.mm ap";
	fullDateFormats << "dd MMM yy hh.mm";
	fullDateFormats << "d MMM h.mm ap";
	fullDateFormats << "d MMM h.mm";
	fullDateFormats << "dd MMM h.mm ap";
	fullDateFormats << "dd MMM h.mm";
	fullDateFormats << "dd MMM hh.mm ap";
	fullDateFormats << "dd MMM hh.mm";
	fullDateFormats << "MMM d h.mm ap";
	fullDateFormats << "MMM d h.mm";
	fullDateFormats << "MMM dd h.mm ap";
	fullDateFormats << "MMM dd h.mm";
	fullDateFormats << "MMM dd hh.mm ap";
	fullDateFormats << "MMM dd hh.mm";

	dateOnlyFormats << "d/MM/yy";
	dateOnlyFormats << "dd/MM/yy";
	dateOnlyFormats << "dd MMM yy";
	dateOnlyFormats << "d MMM yy";
	dateOnlyFormats << "d MMM";
	dateOnlyFormats << "MMM d";
	dateOnlyFormats << "MMM dd";
	
	timeOnlyFormats << "h.mmap";
	timeOnlyFormats << "h.mm ap";
	timeOnlyFormats << "h.mm";
	timeOnlyFormats << "hh.mmap";
	timeOnlyFormats << "hh.mm";
	timeOnlyFormats << "hap";
	timeOnlyFormats << "h ap";
	timeOnlyFormats << "h";
	timeOnlyFormats << "hh ap";
	timeOnlyFormats << "hh";
	


	for (int i=0; i<fullDateFormats.size(); i++) {
		retVal = QDateTime::fromString(dateString, fullDateFormats[i]);
		if (retVal.isValid()) {
			return retVal;
		}
	}

	for (int i=0; i<dateOnlyFormats.size(); i++) {
		retVal = QDateTime::fromString(dateString, dateOnlyFormats[i]);
		if (retVal.isValid()) {
			return retVal;
		}
	}

	QDate currentDate = QDate::currentDate();
	QTime time;
	for (int i=0; i<timeOnlyFormats.size(); i++) {
		time = QTime::fromString(dateString, timeOnlyFormats[i]);
		if (time.isValid()) {
			retVal.setDate(currentDate);
			retVal.setTime(time);
			return retVal;
		}
	}

	return retVal;
}