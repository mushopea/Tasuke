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
	subbedText = subbedText.replace(" by ", " @ ");
	subbedText = subbedText.replace(" at ", " @ ");
	subbedText = subbedText.replace(" from ", " @ ");
	subbedText = subbedText.replace(" on ", " @ ");
	subbedText = subbedText.replace(" to ", " - ");

	subbedText = subbedText.replace(" \\by ", " by ");
	subbedText = subbedText.replace(" \\at ", " at ");
	subbedText = subbedText.replace(" \\from ", " from ");
	subbedText = subbedText.replace(" \\on ", " on ");
	subbedText = subbedText.replace(" \\to ", " to ");

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
	typeKeywords << "settings";
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
	} else if (commandType == "settings") {
		doSettings();
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
void Interpreter::doSettings() {
	Tasuke::instance().showSettingsWindow();
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

	if (dateString == "today") {
		return QDateTime(QDate::currentDate(), QTime(23,59));
	} else if (dateString == "2day") {
		return QDateTime(QDate::currentDate(),  QTime(23,59));
	} else if (dateString == "now") {
		return QDateTime::currentDateTime();
	} else if (dateString == "tomorrow") {
		return QDateTime(QDate::currentDate(), QTime(23,59)).addDays(1);
	} else if (dateString == "tmr") {
		return QDateTime(QDate::currentDate(),  QTime(23,59)).addDays(1);
	} else if (dateString == "day after tomorrow") {
		return QDateTime(QDate::currentDate(),  QTime(23,59)).addDays(2);
	}

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

	// ============================ musho addition start ====================================
	// Fazli, please integrate this with the rest if you approve of them. 
	// I am adding these to test my GUI more efficiently coz i keep screwing dates D:

	// fullDateFormats
	// No space between hours and am/pm
	fullDateFormats << "d/M/yy h.mmap";
	fullDateFormats << "dd/MM/yy h.mmap";
	fullDateFormats << "dd/MM/yy hh.mmap";
	fullDateFormats << "d/M h.mmap";
	fullDateFormats << "dd/MM h.mmap";
	fullDateFormats << "dd/MM hh.mmap";
	fullDateFormats << "d MMM yy h.mmap";
	fullDateFormats << "dd MMM yy h.mmap";
	fullDateFormats << "dd MMM yy hh.mmap";
	fullDateFormats << "d MMM h.mmap";
	fullDateFormats << "dd MMM h.mmap";
	fullDateFormats << "dd MMM hh.mmap";
	fullDateFormats << "MMM d h.mmap";
	fullDateFormats << "MMM dd h.mmap";
	fullDateFormats << "MMM dd hh.mmap";
	// 4 digit years
	fullDateFormats << "dd/MM/yyyy h.mm ap";
	fullDateFormats << "dd/MM/yyyy h.mm";
	fullDateFormats << "dd/MM/yyyy hh.mm ap";
	fullDateFormats << "dd/MM/yyyy hh.mm";
	fullDateFormats << "d MMM yyyy h.mm ap";
	fullDateFormats << "d MMM yyyy h.mm";
	fullDateFormats << "dd MMM yyyy h.mm ap";
	fullDateFormats << "dd MMM yyyy h.mm";
	fullDateFormats << "dd MMM yyyy hh.mm ap";
	fullDateFormats << "dd MMM yyyy hh.mm";
	fullDateFormats << "d/M/yyyy h.mmap";
	fullDateFormats << "dd/MM/yyyy h.mmap";
	fullDateFormats << "dd/MM/yyyy hh.mmap";
	fullDateFormats << "d MMM yyyy h.mmap";
	fullDateFormats << "dd MMM yyyy h.mmap";
	fullDateFormats << "dd MMM yyyy hh.mmap";
	// fully spelt months 
	fullDateFormats << "d MMMM yy h.mm ap";
	fullDateFormats << "d MMMM yy h.mm";
	fullDateFormats << "dd MMMM yy h.mm ap";
	fullDateFormats << "dd MMMM yy h.mm";
	fullDateFormats << "dd MMMM yy hh.mm ap";
	fullDateFormats << "dd MMMM yy hh.mm";
	fullDateFormats << "d MMMM h.mm ap";
	fullDateFormats << "d MMMM h.mm";
	fullDateFormats << "dd MMMM h.mm ap";
	fullDateFormats << "dd MMMM h.mm";
	fullDateFormats << "dd MMMM hh.mm ap";
	fullDateFormats << "dd MMMM hh.mm";
	fullDateFormats << "MMMM d h.mm ap";
	fullDateFormats << "MMMM d h.mm";
	fullDateFormats << "MMMM dd h.mm ap";
	fullDateFormats << "MMMM dd h.mm";
	fullDateFormats << "MMMM dd hh.mm ap";
	fullDateFormats << "MMMM dd hh.mm";
	fullDateFormats << "d MMMM yy h.mmap";
	fullDateFormats << "dd MMMM yy h.mmap";
	fullDateFormats << "dd MMMM yy hh.mmap";
	fullDateFormats << "d MMMM h.mmap";
	fullDateFormats << "dd MMMM h.mmap";
	fullDateFormats << "dd MMMM hh.mmap";
	fullDateFormats << "MMMM d h.mmap";
	fullDateFormats << "MMMM dd h.mmap";
	fullDateFormats << "MMMM dd hh.mmap";
	fullDateFormats << "d MMMM yyyy h.mm ap";
	fullDateFormats << "d MMMM yyyy h.mm";
	fullDateFormats << "dd MMMM yyyy h.mm ap";
	fullDateFormats << "dd MMMM yyyy h.mm";
	fullDateFormats << "dd MMMM yyyy hh.mm ap";
	fullDateFormats << "dd MMMM yyyy hh.mm";
	fullDateFormats << "d MMMM yyyy h.mmap";
	fullDateFormats << "dd MMMM yyyy h.mmap";
	fullDateFormats << "dd MMMM yyyy hh.mmap";
	// hours only, no minutes 
	fullDateFormats << "d/M/yy h ap";
	fullDateFormats << "d/M/yy h";
	fullDateFormats << "dd/MM/yy h ap";
	fullDateFormats << "dd/MM/yy h";
	fullDateFormats << "dd/MM/yy hh ap";
	fullDateFormats << "dd/MM/yy hh";
	fullDateFormats << "d/M h ap";
	fullDateFormats << "d/M h";
	fullDateFormats << "dd/MM h ap";
	fullDateFormats << "dd/MM h";
	fullDateFormats << "dd/MM hh ap";
	fullDateFormats << "dd/MM hh";
	fullDateFormats << "d MMM yy h ap";
	fullDateFormats << "d MMM yy h";
	fullDateFormats << "dd MMM yy h ap";
	fullDateFormats << "dd MMM yy h";
	fullDateFormats << "dd MMM yy hh ap";
	fullDateFormats << "dd MMM yy h";
	fullDateFormats << "d MMM h ap";
	fullDateFormats << "d MMM h";
	fullDateFormats << "dd MMM h ap";
	fullDateFormats << "dd MMM h";
	fullDateFormats << "dd MMM hh ap";
	fullDateFormats << "dd MMM hh";
	fullDateFormats << "MMM d h ap";
	fullDateFormats << "MMM d h";
	fullDateFormats << "MMM dd h ap";
	fullDateFormats << "MMM dd h";
	fullDateFormats << "MMM dd hh ap";
	fullDateFormats << "MMM dd hh";
	fullDateFormats << "d/M/yy hap";
	fullDateFormats << "dd/MM/yy hap";
	fullDateFormats << "dd/MM/yy hhap";
	fullDateFormats << "d/M hap";
	fullDateFormats << "dd/MM hap";
	fullDateFormats << "dd/MM hhap";
	fullDateFormats << "d MMM yy hap";
	fullDateFormats << "dd MMM yy hap";
	fullDateFormats << "dd MMM yy hhap";
	fullDateFormats << "d MMM hap";
	fullDateFormats << "dd MMM hap";
	fullDateFormats << "dd MMM hhap";
	fullDateFormats << "MMM d hap";
	fullDateFormats << "MMM dd hap";
	fullDateFormats << "MMM dd hhap";
	fullDateFormats << "dd/MM/yyyy h ap";
	fullDateFormats << "dd/MM/yyyy h";
	fullDateFormats << "dd/MM/yyyy hh ap";
	fullDateFormats << "dd/MM/yyyy hh";
	fullDateFormats << "d MMM yyyy h ap";
	fullDateFormats << "d MMM yyyy h";
	fullDateFormats << "dd MMM yyyy h ap";
	fullDateFormats << "dd MMM yyyy h";
	fullDateFormats << "dd MMM yyyy hh ap";
	fullDateFormats << "dd MMM yyyy hh";
	fullDateFormats << "d/M/yyyy hap";
	fullDateFormats << "dd/MM/yyyy hap";
	fullDateFormats << "dd/MM/yyyy hhap";
	fullDateFormats << "d MMM yyyy hap";
	fullDateFormats << "dd MMM yyyy hap";
	fullDateFormats << "dd MMM yyyy hhap";
	fullDateFormats << "d MMMM yy h ap";
	fullDateFormats << "d MMMM yy h";
	fullDateFormats << "dd MMMM yy h ap";
	fullDateFormats << "dd MMMM yy h";
	fullDateFormats << "dd MMMM yy hh ap";
	fullDateFormats << "dd MMMM yy hh";
	fullDateFormats << "d MMMM h ap";
	fullDateFormats << "d MMMM h";
	fullDateFormats << "dd MMMM h ap";
	fullDateFormats << "dd MMMM h";
	fullDateFormats << "dd MMMM hh ap";
	fullDateFormats << "dd MMMM hh";
	fullDateFormats << "MMMM d h ap";
	fullDateFormats << "MMMM d h";
	fullDateFormats << "MMMM dd h ap";
	fullDateFormats << "MMMM dd h";
	fullDateFormats << "MMMM dd hh ap";
	fullDateFormats << "MMMM dd hh";
	fullDateFormats << "d MMMM yy hap";
	fullDateFormats << "dd MMMM yy hap";
	fullDateFormats << "dd MMMM yy hhap";
	fullDateFormats << "d MMMM hap";
	fullDateFormats << "dd MMMM hap";
	fullDateFormats << "dd MMMM hhap";
	fullDateFormats << "MMMM d hap";
	fullDateFormats << "MMMM dd hap";
	fullDateFormats << "MMMM dd hhap";
	fullDateFormats << "d MMMM yyyy h ap";
	fullDateFormats << "d MMMM yyyy h";
	fullDateFormats << "dd MMMM yyyy h ap";
	fullDateFormats << "dd MMMM yyyy h";
	fullDateFormats << "dd MMMM yyyy hh ap";
	fullDateFormats << "dd MMMM yyyy hh";
	fullDateFormats << "d MMMM yyyy hap";
	fullDateFormats << "dd MMMM yyyy hap";
	fullDateFormats << "dd MMMM yyyy hhap";


	// dateOnlyFormats
	// one digit months
	dateOnlyFormats << "d/M/yy";
	dateOnlyFormats << "dd/M/yy";
	// four digit years
	dateOnlyFormats << "d/M/yyyy";
	dateOnlyFormats << "dd/M/yyyy";
	dateOnlyFormats << "dd MMM yyyy";
	dateOnlyFormats << "d MMM yyyy";
	// fully spelt months
	dateOnlyFormats << "dd MMM";
	dateOnlyFormats << "dd MMMM yyyy";
	dateOnlyFormats << "d MMMM yyyy";
	dateOnlyFormats << "dd MMMM";
	dateOnlyFormats << "dd MMMM yy";
	dateOnlyFormats << "d MMMM yy";
	dateOnlyFormats << "d MMMM";
	dateOnlyFormats << "MMMM d";
	dateOnlyFormats << "MMMM dd";
	//no years
	dateOnlyFormats << "d/M";
	dateOnlyFormats << "d/MM";
	dateOnlyFormats << "dd/M";
	dateOnlyFormats << "dd/MM";

	// ============================ musho addition end ====================================

	for (int i=0; i<fullDateFormats.size(); i++) {
		retVal = QDateTime::fromString(dateString, fullDateFormats[i]);
		if (retVal.isValid()) {
			// Musho attempt to fix date bug:
			// Once the date string has been successfully matched with a format string, check
			// If that format string does not contain a year. If it doesn't, the QDateTime
			// Object will default to the year 1900, so move the year forward by currentYear - 1900.
			if (!fullDateFormats[i].contains("yy")) {
				QString currentYearString = QDate::currentDate().toString("yyyy");
				retVal = retVal.addYears(currentYearString.toInt() - 1900);
			}
			return retVal;
		}
	}

	for (int i=0; i<dateOnlyFormats.size(); i++) {
		retVal = QDateTime::fromString(dateString, dateOnlyFormats[i]);
		if (retVal.isValid()) {
			if (!dateOnlyFormats[i].contains("yy")) {
				QString currentYearString = QDate::currentDate().toString("yyyy");
				retVal = retVal.addYears(currentYearString.toInt() - 1900);
			}
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