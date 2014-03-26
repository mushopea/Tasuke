#include <cassert>
#include <glog/logging.h>
#include <QApplication>
#include <QString>
#include <QStringList>
#include "Tasuke.h"
#include "Commands.h"
#include "Constants.h"
#include "Exceptions.h"
#include "Interpreter.h"

QString Interpreter::substitute(QString text) {
	QString subbedText = text;
	subbedText = subbedText.replace("by", "@");
	subbedText = subbedText.replace("at", "@");
	subbedText = subbedText.replace("from", "@");
	subbedText = subbedText.replace("to", "-");

	subbedText = subbedText.replace("\by", "by");
	subbedText = subbedText.replace("\at", "at");
	subbedText = subbedText.replace("\from", "from");
	subbedText = subbedText.replace("\to", "to");

	return subbedText;
}

QHash<QString, QString> Interpreter::decompose(QString text) {
	QStringList tokens = text.split(" ");
	QString current = "";
	bool expectNewDelimiter = false;
	QHash<QString, QString> retVal;

	for (int i=0; i<tokens.size(); i++) {
		if (tokens[i].size() > 0 && (tokens[i][0] == '@' || tokens[i][0] == '#' || tokens[i] == "-@" || tokens[i].startsWith("-#"))) {
			current = tokens[i];

			if (tokens[i][0] == '@') {
				current = tokens[i][0];
				tokens[i].remove(0,1);

				if (retVal.contains(current) == true) {
					throw ExceptionBadCommand();
				}
			} else if (tokens[i][0] == '#') {
				tokens[i].remove(0,1);
				expectNewDelimiter = true;
			} else if (tokens[i] == "-@") {
				tokens[i].remove(0,2);
				expectNewDelimiter = true;
			} else if (tokens[i].startsWith("-#")) {
				tokens[i].remove(0,2);
				expectNewDelimiter = true;
			}
		} else {
			if (expectNewDelimiter) {
				throw ExceptionBadCommand();
			}
		}

		expectNewDelimiter = false;

		QString newVal =  tokens[i];
		QString temp = retVal[current];
		if (temp.isEmpty() == false) {
			newVal = temp + " " + newVal;
		}
		retVal[current] = newVal;
	}

	return retVal;
}

QString Interpreter::removeBefore(QString text, QString before) {
	QString retVal = text;
	int pos = retVal.indexOf(before);

	if (pos != -1) {
		retVal.remove(0, pos + before.size());
	}

	return retVal;
}

QString Interpreter::getType(QString commandString) {
	QStringList delimiters;
	QStringList typeKeywords;

	delimiters << "@";
	delimiters << "#";
	delimiters << "-";

	typeKeywords << "add";
	typeKeywords << "edit";
	typeKeywords << "remove";
	typeKeywords << "show";
	typeKeywords << "hide";
	typeKeywords << "done";
	typeKeywords << "undone";
	typeKeywords << "undo";
	typeKeywords << "redo";
	typeKeywords << "clear";
	typeKeywords << "help";
	typeKeywords << "about";
	typeKeywords << "exit";
	
	QString temp = commandString.trimmed();
	
	for (int i=0; i<delimiters.size(); i++) {
		temp = temp.split(delimiters[i])[0];
	}
	QStringList tokens = temp.split(' ');
	for (int i=0; i<tokens.size(); i++) {
		for (int j=0; j<typeKeywords.size(); j++) {
			if (tokens[i] == typeKeywords[j]) {
				return typeKeywords[j];
			}
		}
	}

	return "";
}

// This static helper function returns an instance of a ICommand that represents
// the user's command. The caller must clean up using delete.
ICommand* Interpreter::interpret(QString commandString) {
	LOG(INFO) << "Interpretting " << commandString.toStdString();

	commandString = substitute(commandString);

	QString commandType = getType(commandString);

	if (commandType == "add") {
		return createAddCommand(commandString);
	} else if (commandType == "remove") {
		return createRemoveCommand(commandString);
	} else if (commandType == "edit") {
		return createEditCommand(commandString);
	} else if (commandType == "done") {
		return createDoneCommand(commandString);
	} else if (commandType == "undone") {
		return createUndoneCommand(commandString);
	}

	if (commandType == "") {
		throw ExceptionBadCommand();
	}
	
	if (commandType == "show") {
		doShow();
	} else if (commandType == "hide") {
		doHide();
	} else if (commandType == "undo") {
		doUndo();
	} else if (commandType == "redo") {
		doRedo();
	} else if (commandType == "clear") {
		return createClearCommand(commandString);
	} else if (commandType == "help") {
		doHelp();
	} else if (commandType == "about") {
		doAbout();
	} else if (commandType == "exit") {
		doExit();
	}

	return nullptr;
}

AddCommand* Interpreter::createAddCommand(QString commandString) {
	commandString = removeBefore(commandString, "add");
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand();
	}

	QHash<QString, QString> parts = decompose(commandString);
	Task task;

	task.setDescription(parts[""]);

	foreach(const QString &key, parts.keys()) {
		QString value = parts[key].trimmed();

		if (key.startsWith('#')) {
			task.addTag(value);
		} else if (key == "@") {
			TIME_PERIOD period = parseTimePeriod(value);
			task.setBegin(period.begin);
			task.setEnd(period.end);
		}
	}

	return new AddCommand(task);
}

RemoveCommand* Interpreter::createRemoveCommand(QString commandString) {
	commandString = removeBefore(commandString, "remove");
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand();
	}

	QString idString = commandString.split(' ')[0];
	int id = parseId(idString);

	return new RemoveCommand(id-1);
}

EditCommand* Interpreter::createEditCommand(QString commandString) {
	commandString = removeBefore(commandString, "edit");
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand();
	}

	QString idString = commandString.split(' ')[0];
	int id = parseId(idString);

	commandString = commandString.section(' ', 1);

	QHash<QString, QString> parts = decompose(commandString);
	Task task = Tasuke::instance().getStorage().getTask(id-1);

	foreach(const QString &key, parts.keys()) {
		QString value = parts[key].trimmed();

		if (key == "") {
			task.setDescription(value);
		} else if (key.startsWith('#')) {
			task.addTag(value);
		} else if (key == "@") {
			TIME_PERIOD period = parseTimePeriod(value);
			task.setBegin(period.begin);
			task.setEnd(period.end);
		} else if (key.startsWith("-#")) {
			task.removeTag(value);
		} else if (key.startsWith("-@")) {
			task.setBegin(QDateTime());
			task.setEnd(QDateTime());
		}
	}

	return new EditCommand(id-1, task);
}

ClearCommand* Interpreter::createClearCommand(QString commandString) {
	return new ClearCommand();
}

DoneCommand* Interpreter::createDoneCommand(QString commandString) {
	commandString = removeBefore(commandString, "done");
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand();
	}

	QString idString = commandString.split(' ')[0];
	int id = parseId(idString);

	return new DoneCommand(id-1);
}
DoneCommand* Interpreter::createUndoneCommand(QString commandString) {
	commandString = removeBefore(commandString, "undone");
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand();
	}

	QString idString = commandString.split(' ')[0];
	int id = parseId(idString);

	return new DoneCommand(id-1, false);
}

void Interpreter::doShow() {
	Tasuke::instance().showTaskWindow();
}
void Interpreter::doAbout() {
	Tasuke::instance().showAboutWindow();
}
void Interpreter::doHide() {
	Tasuke::instance().hideTaskWindow();
}
void Interpreter::doUndo() {
	Tasuke::instance().undoCommand();
}
void Interpreter::doRedo() {
	Tasuke::instance().redoCommand();
}
void Interpreter::doHelp() {
	Tasuke::instance().showTutorial();
}
void Interpreter::doExit() {
	QApplication::quit();
}

int Interpreter::parseId(QString idString) {
	bool ok = false;
	int id = idString.toInt(&ok);

	if (ok == false) {
		throw ExceptionBadCommand();
	}

	int numTasks = Tasuke::instance().getStorage().totalTasks();

	if (id < 1 || id > numTasks) {
		throw ExceptionBadCommand();
	}

	return id;
}

Interpreter::TIME_PERIOD Interpreter::parseTimePeriod(QString timePeriod) {
	QStringList timePeriodParts = timePeriod.split("-");
	TIME_PERIOD retVal;

	if (timePeriodParts.size() > 2) {
		throw ExceptionBadCommand();
	}

	if (timePeriodParts.size() == 1) {
		retVal.end = parseDate(timePeriod);
	} else if (timePeriodParts.size() == 2) {
		retVal.begin = parseDate(timePeriodParts[0]);
		retVal.end = parseDate(timePeriodParts[1]);
	}

	return retVal;
}

QDateTime Interpreter::parseDate(QString dateString) {
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