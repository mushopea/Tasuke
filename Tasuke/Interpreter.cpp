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

bool Interpreter::formatsAlreadyInit = false;
QStringList Interpreter::timeFormats;
QStringList Interpreter::dateFormatsWithoutYear;
QStringList Interpreter::dateFormats;
QStringList Interpreter::dateTimeFormats;


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
		retVal.begin = parseDate(timePeriodParts[0], false);
		retVal.end = parseDate(timePeriodParts[1]);

		if (!retVal.begin.isValid()) {
			throw ExceptionBadCommand();
		}
	}

	if (!retVal.end.isValid()) {
		throw ExceptionBadCommand();
	}

	if (retVal.begin.isValid() && retVal.end < retVal.begin) {
		throw ExceptionBadCommand();
	}

	return retVal;
}

QDateTime Interpreter::parseDate(QString dateString, bool isEnd) {
	dateString = dateString.trimmed();

	QDate currentDate = QDate::currentDate();
	QTime timePart = QTime(23,59);
	if (!isEnd) {
		QTime timePart = QTime(0,0);
	}

	if (dateString == "today") {
		return QDateTime(currentDate, timePart);
	} else if (dateString == "2day") {
		return QDateTime(currentDate, timePart);
	} else if (dateString == "now") {
		return QDateTime::currentDateTime();
	} else if (dateString == "tomorrow") {
		return QDateTime(currentDate, timePart).addDays(1);
	} else if (dateString == "tmr") {
		return QDateTime(currentDate, timePart).addDays(1);
	} else if (dateString == "day after tomorrow") {
		return QDateTime(currentDate, timePart).addDays(2);
	}

	if (!formatsAlreadyInit) {
		initFormats();
	}

	QDateTime retVal;

	// these formats are complete
	foreach(QString dateTimeFormat, dateTimeFormats) {
		retVal = QDateTime::fromString(dateString, dateTimeFormat);
		if (retVal.isValid()) {
			QDate date = retVal.date();
			if (date.year() < 2000) {
				// add a century
				date = date.addYears(100);
				retVal.setDate(date);
			}
			return retVal;
		}
	}

	// these formats need the time added
	foreach(QString dateFormat, dateFormats) {
		retVal = QDateTime::fromString(dateString, dateFormat);
		if (retVal.isValid()) {
			QDate date = retVal.date();
			if (date.year() < 2000) {
				// add a century
				date = date.addYears(100);
				retVal.setDate(date);
			}
			retVal.setTime(timePart);
			return retVal;
		}
	}


	// these formats need the current year added
	foreach(QString dateFormat, dateFormatsWithoutYear) {
		retVal = QDateTime::fromString(dateString, dateFormat);
		if (retVal.isValid()) {
			QDate date = retVal.date();
			retVal.setDate(QDate(currentDate.year(), date.month(), date.day()));
			return retVal;
		}
	}

	// these formats need the date added
	foreach(QString timeFormat, timeFormats) {
		timePart = QTime::fromString(dateString, timeFormat);
		if (timePart.isValid()) {
			retVal.setDate(currentDate);
			retVal.setTime(timePart);
			return retVal;
		}
	}

	return retVal;
}

void Interpreter::initFormats() {
	if (formatsAlreadyInit) {
		return;
	}

	generateTimeFormats();
	generateDateFormatsWithoutYear();
	generateDateFormats();
	generateDateTimeFormats();

	formatsAlreadyInit = true;
}

void Interpreter::generateTimeFormats() {
	assert(formatsAlreadyInit == false);

	QStringList hourFormats;
	QStringList minuteFomats;
	QStringList amPmFormats;
	QStringList separators;
	QStringList optionalSpaces;
	
	hourFormats << "h" << "hh";
	minuteFomats << "mm";
	amPmFormats << "ap" << "AP";
	separators << " " << ":" << ".";
	optionalSpaces << " " << "";

	timeFormats << hourFormats;
	QStringList toAdd;
	foreach(QString timeFormat, timeFormats) {
		foreach(QString minuteFormat, minuteFomats) {
			foreach(QString separator, separators) {
				toAdd << (timeFormat + separator + minuteFormat);
			}
		}
	}
	timeFormats << toAdd;
	toAdd.clear();
	foreach(QString timeFormat, timeFormats) {
		foreach(QString amPmFormat, amPmFormats) {
			foreach(QString optionalSpace, optionalSpaces) {
				toAdd << (timeFormat + optionalSpace + amPmFormat);
			}
		}
	}
	timeFormats << toAdd;

	// special constructions
	// military time:
	timeFormats << "hhmm'hrs'";;
}
void Interpreter::generateDateFormatsWithoutYear() {
	assert(formatsAlreadyInit == false);

	QStringList dayFormats;
	QStringList speltMonthFormats;
	QStringList monthFormats;
	QStringList dateSeparators;
	QStringList spaceOrCommas;

	dayFormats << "d" << "dd";
	speltMonthFormats << "MMM" << "MMMM";
	monthFormats << "M" << "MM";
	spaceOrCommas << " " << ", ";
	dateSeparators << "/" << "-"; 

	foreach(QString dayFormat, dayFormats) {
		foreach(QString speltMonthFormat, speltMonthFormats) {
			foreach(QString spaceOrComma, spaceOrCommas) {
				dateFormatsWithoutYear << (dayFormat + spaceOrComma + speltMonthFormat);
				// american format:
				dateFormatsWithoutYear << (speltMonthFormat + spaceOrComma + dayFormat);
			}
		}
	}

	//special constructions
	foreach(QString dayFormat, dayFormats) {
		foreach(QString monthFormat, monthFormats) {
			foreach(QString dateSeparator, dateSeparators) {
				dateFormatsWithoutYear << (dayFormat + dateSeparator + monthFormat);
			}
		}
	}
}
void Interpreter::generateDateFormats() {
	assert(formatsAlreadyInit == false);

	QStringList dayFormats;
	QStringList monthFormats;
	QStringList yearFormats;
	QStringList spaceOrCommas;
	QStringList dateSeparators;

	dayFormats << "d" << "dd";
	monthFormats << "M" << "MM";
	yearFormats << "yy" << "yyyy";
	spaceOrCommas << " " << ", ";
	dateSeparators << "/" << "-"; 

	foreach(QString dateFormatWithoutYear, dateFormatsWithoutYear) {
		foreach(QString yearFormat, yearFormats) {
			foreach(QString spaceOrComma, spaceOrCommas) {
				dateFormats << (dateFormatWithoutYear + spaceOrComma + yearFormat);
				dateFormats << (yearFormat + spaceOrComma + dateFormatWithoutYear);
			}
		}
	}

	// special construction
	foreach(QString dayFormat, dayFormats) {
		foreach(QString monthFormat, monthFormats) {
			foreach(QString yearFormat, yearFormats) {
				foreach(QString dateSeparator, dateSeparators) {
					dateFormats << (dayFormat + dateSeparator + monthFormat + dateSeparator + yearFormat);
				}
			}
		}
	}
}
void Interpreter::generateDateTimeFormats() {
	assert(formatsAlreadyInit == false);

	QStringList spaceOrCommas;
	spaceOrCommas << " " << ", ";

	foreach(QString dateFormat, dateFormats) {
		foreach(QString timeFormat, timeFormats) {
			foreach(QString spaceOrComma, spaceOrCommas) {
				dateTimeFormats << (dateFormat + spaceOrComma + timeFormat);
				dateTimeFormats << (timeFormat + spaceOrComma + dateFormat);
			}
		}
	}
}