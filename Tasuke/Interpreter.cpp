//@author A0096836M

#include <cassert>
#include <glog/logging.h>
#include <QApplication>
#include <QString>
#include <QStringList>
#include <QMutexLocker>
#include "Tasuke.h"
#include "Commands.h"
#include "Constants.h"
#include "Exceptions.h"
#include "Interpreter.h"

QMutex Interpreter::mutex;

int Interpreter::last = -1;
bool Interpreter::formatsAlreadyInit = false;

QStringList Interpreter::timeFormats;
QStringList Interpreter::dateFormatsWithoutYear;
QStringList Interpreter::dateFormats;
QStringList Interpreter::dateTimeFormats;
QStringList Interpreter::dateTimeFormatsWithoutYear;

QStringList Interpreter::timeFormatsAp;
QStringList Interpreter::dateTimeFormatsAp;
QStringList Interpreter::dateTimeFormatsWithoutYearAp;

// sets the last task id
void Interpreter::setLast(int _last) {
	last = _last;
}

// substitutes parts of command with understandable equivalents
QString Interpreter::substitute(QString text) {
	QString subbedText = text;
	foreach(QRegExp regex, EQUIV_AT_REGEX) {
		subbedText.replace(regex, EQUIV_AT_REPLACE);
	}

	for (int i=0; i<COMMANDS.size(); i++) {
		foreach(QRegExp regex, EQUIV_COMMAND_REGEX[i]) {
			subbedText.replace(regex, COMMANDS[i]);
		}
	}

	return subbedText;
}

// substitute parts of ranges with undeertandable equivalents
QString Interpreter::substituteForRange(QString text) {
	QString subbedText = text;
	subbedText.replace(EQUIV_TO_REGEX, DELIMITER_DASH);
	subbedText.remove(KEYWORD_BACKSLASH);

	return subbedText;
}

// substitute parts of descriptions with understandable equivalents
QString Interpreter::substituteForDescription(QString text) {
	QString subbedText = text;
	subbedText.remove(KEYWORD_BACKSLASH);

	return subbedText;
}

// gets the date of the upcomming weekday. 1 = Monday, 7 = Sunday
QDate Interpreter::nextWeekday(int weekday) {
	assert(weekday >= 1 && weekday <= 7);

	QDate date = QDate::currentDate();
	
	while (date.dayOfWeek() != weekday) {
		date = date.addDays(1);
	}

	return date;
}

// substitute parts of dates with understandable equivalents
QString Interpreter::substituteForDate(QString text) {
	QString subbedText = text.toLower();

	// strip commas
	foreach(QRegExp regex, REMOVE_DATE_REGEX) {
		subbedText.remove(regex);
	}

	// expand out abbrev
	subbedText.replace(DAY_2DAY_REGEX, DAY_TODAY);
	subbedText.replace(DAY_TMR_REGEX, DAY_TOMORROW);
	subbedText.replace(DAY_TML_REGEX, DAY_TOMORROW);
	subbedText.replace(DAY_MON_REGEX, DAY_MONDAY);
	subbedText.replace(DAY_TUE_REGEX, DAY_TUESDAY);
	subbedText.replace(DAY_TUES_REGEX, DAY_TUESDAY);
	subbedText.replace(DAY_WED_REGEX, DAY_WEDNESDAY);
	subbedText.replace(DAY_THU_REGEX, DAY_THURSDAY);
	subbedText.replace(DAY_THUR_REGEX, DAY_THURSDAY);
	subbedText.replace(DAY_THURS_REGEX, DAY_THURSDAY);
	subbedText.replace(DAY_FRI_REGEX, DAY_FRIDAY);
	subbedText.replace(DAY_SAT_REGEX, DAY_SATURDAY);
	subbedText.replace(DAY_SUN_REGEX, DAY_SUNDAY);

	// today, tomorrow etc.
	subbedText.replace(DAY_YESTERDAY_REGEX, DATE_YESTERDAY);
	subbedText.replace(DAY_TODAY_REGEX, DATE_TODAY);
	subbedText.replace(DAY_TOMORROW_REGEX, DATE_TOMORROW);
	subbedText.replace(DAY_AFTER_TOMORROW_REGEX, DATE_AFTER_TOMORROW);

	// weekdays
	for (int i=0; i<DAY_NAMES.size(); i++) {
		subbedText.replace(DAY_NAMES[i],
			nextWeekday(i+1).toString(DATE_FORMAT));
	}

	// time of days
	for (int i=0; i<TIME_NAMED.size(); i++) {
		subbedText.replace(TIME_NAMES_REGEX[i], TIME_NAMED[i]);
	}

	return subbedText;
}

// decompose a command so that it is easy to parse
// throws ExceptionBadCommand if unable to parse
QHash<QString, QString> Interpreter::decompose(QString text) {
	QStringList tokens = text.split(" ");
	QString current = "";
	bool expectNewDelimiter = false;
	QHash<QString, QString> retVal;

	for (int i=0; i<tokens.size(); i++) {
		if (tokens[i].size() > 0 && (tokens[i][0] == CHAR_DELIMITER_AT
			|| tokens[i][0] == CHAR_DELIMITER_HASH
			|| tokens[i] == DELIMITER_DASH_AT
			|| tokens[i].startsWith(DELIMITER_DASH_HASH))) {
			current = tokens[i];
			expectNewDelimiter = false;

			if (tokens[i][0] == CHAR_DELIMITER_AT) {
				// reach @ delimieter
				current = tokens[i][0];
				tokens[i].remove(0,1);

				if (retVal.contains(current) == true) {
					throw ExceptionBadCommand(ERROR_MULTIPLE_DATES, WHERE_DATE);
				}
			} else if (tokens[i][0] == CHAR_DELIMITER_HASH) {
				// reach # delimiter
				tokens[i].remove(0,1);
				expectNewDelimiter = true;

				if (tokens[i].isEmpty()) {
					throw ExceptionBadCommand(ERROR_TAG_NO_NAME, WHERE_TAG);
				}
			} else if (tokens[i] == DELIMITER_DASH_AT) {
				// reach -@ delimiter
				tokens[i].remove(0,2);
				expectNewDelimiter = true;
			} else if (tokens[i].startsWith(DELIMITER_DASH_HASH)) {
				// reach -# delimiter
				tokens[i].remove(0,2);
				expectNewDelimiter = true;

				if (tokens[i].isEmpty()) {
					throw ExceptionBadCommand(ERROR_TAG_REMOVE_NO_NAME, WHERE_TAG);
				}
			}
		} else {
			// didn't expect this token here
			if (expectNewDelimiter) {
				throw ExceptionBadCommand(ERROR_DONT_KNOW(tokens[i]), WHERE_DESCRIPTION);
			}
		}

		// add token to correct key
		QString newVal =  tokens[i];
		QString temp = retVal[current];
		if (temp.isEmpty() == false) {
			newVal = temp + " " + newVal;
		}
		retVal[current] = newVal;
	}

	return retVal;
}

// removes anything before in the text
QString Interpreter::removeBefore(QString text, QString before) {
	QString retVal = text;
	int pos = retVal.indexOf(before);

	if (pos != -1) {
		retVal.remove(0, pos + before.size());
	}

	return retVal;
}

// Trys to guess the type of the command.
// If the type is cannot be determined return COMMAND_NIL.
// Defaults to perform substitutions but can be disabled
QString Interpreter::getType(QString commandString, bool doSub) {
	if (doSub) {
		commandString = substitute(commandString);
	}
	
	QString temp = commandString.trimmed();

	foreach(QString delimiter, DELIMITERS) {
		temp = temp.split(delimiter)[0];
	}

	QStringList tokens = temp.split(' ');
	foreach(QString token, tokens) {
		foreach(QString keyword, COMMANDS) {
			if (token == keyword) {
				return keyword;
			}
		}
	}

	return COMMAND_NIL;
}

// This static helper function returns an instance of a ICommand that represents
// the user's command. The caller must clean up using delete. Takes in the
// user input and a boolean dry. If dry is true, nothing is actually done.
// defaults to false.
// throws ExceptionBadCommand if unable to parse
ICommand* Interpreter::interpret(QString commandString, bool dry) {
	LOG(INFO) << MSG_INTERPRETER_INTERPRETTING(commandString);

	commandString = substitute(commandString);

	QString commandType = getType(commandString, false);

	// these commands need to be parsed if valid
	if (commandType == COMMAND_NIL) {
		throw ExceptionBadCommand(ERROR_DONT_UNDERSTAND);
	} else if (commandType == COMMAND_ADD) {
		return createAddCommand(commandString);
	} else if (commandType == COMMAND_REMOVE) {
		return createRemoveCommand(commandString);
	} else if (commandType == COMMAND_EDIT) {
		return createEditCommand(commandString);
	} else if (commandType == COMMAND_DONE) {
		return createDoneCommand(commandString);
	} else if (commandType == COMMAND_UNDONE) {
		return createUndoneCommand(commandString);
	} else if (commandType == COMMAND_CLEAR) {
		return createClearCommand(commandString);
	} else if (commandType == COMMAND_UNDO) {
		doUndo(commandString, dry);
	} else if (commandType == COMMAND_REDO) {
		doRedo(commandString, dry);
	}
	
	// if this was a dry run, don't actually do anything
	if (dry) {
		return nullptr;
	}

	// these commands are always valid
	if (commandType == COMMAND_SHOW) {
		doShow(commandString);
	} else if (commandType == COMMAND_HIDE) {
		doHide();
	} else if (commandType == COMMAND_HELP) {
		doHelp();
	} else if (commandType == COMMAND_SETTINGS) {
		doSettings();
	} else if (commandType == COMMAND_ABOUT) {
		doAbout();
	} else if (commandType == COMMAND_EXIT) {
		doExit();
	}

	// don't need to return a command object
	return nullptr;
}

// creates an add command. takes in a string from user input
// throws ExceptionBadCommand if unable to parse
ICommand* Interpreter::createAddCommand(QString commandString) {
	commandString = removeBefore(commandString, COMMAND_ADD);
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand(ERROR_ADD_EMPTY, WHERE_DESCRIPTION);
	}

	QHash<QString, QString> parts = decompose(commandString);
	Task task;

	QString description = parts[""];
	description = substituteForDescription(description);

	if (description.isEmpty()) {
		throw ExceptionBadCommand(ERROR_NO_DESCRIPTION, WHERE_DESCRIPTION);
	}

	task.setDescription(description);

	foreach(const QString &key, parts.keys()) {
		QString value = parts[key].trimmed();

		if (key.startsWith(DELIMITER_HASH)) {
			task.addTag(value);
		} else if (key == DELIMITER_AT) {
			TIME_PERIOD period = parseTimePeriod(value);
			task.setBegin(period.begin);
			task.setEnd(period.end);
		}
	}

	return new AddCommand(task);
}

// creates an remove command. takes in a string from user input
// throws ExceptionBadCommand if unable to parse
ICommand* Interpreter::createRemoveCommand(QString commandString) {
	commandString = removeBefore(commandString, COMMAND_REMOVE);
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand(ERROR_REMOVE_NO_ID, WHERE_ID);
	}

	QList<int> ids = parseIdList(commandString);
	QList< QSharedPointer<ICommand> > commands;

	qSort(ids.begin(), ids.end(), qGreater<int>());

	foreach(int id, ids) {
		QSharedPointer<ICommand> command = QSharedPointer<ICommand>(new RemoveCommand(id-1));
		commands.push_back(command);
	}

	return new CompositeCommand(commands);
}

// creates an edit command. takes in a string from user input
// throws ExceptionBadCommand if unable to parse
ICommand* Interpreter::createEditCommand(QString commandString) {
	commandString = removeBefore(commandString, COMMAND_EDIT);
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand(ERROR_EDIT_NO_ID, WHERE_ID);
	}

	QString idString = commandString.split(' ')[0];
	int id = parseId(idString);

	commandString = commandString.section(' ', 1);

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand(ERROR_EDIT_EMPTY, WHERE_DESCRIPTION);
	}

	QHash<QString, QString> parts = decompose(commandString);
	Task task = Tasuke::instance().getStorage().getTask(id-1);

	foreach(const QString &key, parts.keys()) {
		QString value = parts[key].trimmed();

		if (key == "") {
			value = substituteForDescription(value);
			task.setDescription(value);
		} else if (key.startsWith(DELIMITER_HASH)) {
			task.addTag(value);
		} else if (key == DELIMITER_AT) {
			TIME_PERIOD period = parseTimePeriod(value);
			task.setBegin(period.begin);
			task.setEnd(period.end);
		} else if (key.startsWith(DELIMITER_DASH_HASH)) {
			task.removeTag(value);
		} else if (key.startsWith(DELIMITER_DASH_AT)) {
			task.setBegin(QDateTime());
			task.setEnd(QDateTime());
		}
	}

	return new EditCommand(id-1, task);
}

// creates a clear command. takes in a string from user input
// throws ExceptionBadCommand if unable to parse
ICommand* Interpreter::createClearCommand(QString commandString) {
	return new ClearCommand();
}

// creates a done command. takes in a string from user input
// throws ExceptionBadCommand if unable to parse
ICommand* Interpreter::createDoneCommand(QString commandString) {
	commandString = removeBefore(commandString, COMMAND_DONE);
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand(ERROR_DONE_NO_ID, WHERE_ID);
	}

	QList<int> ids = parseIdList(commandString);
	QList< QSharedPointer<ICommand> > commands;

	qSort(ids.begin(), ids.end(), qGreater<int>());

	foreach(int id, ids) {
		QSharedPointer<ICommand> command = QSharedPointer<ICommand>(new DoneCommand(id-1));
		commands.push_back(command);
	}

	return new CompositeCommand(commands);
}

// creates an undone command. takes in a string from user input
// throws ExceptionBadCommand if unable to parse
ICommand* Interpreter::createUndoneCommand(QString commandString) {
	commandString = removeBefore(commandString, COMMAND_UNDONE);
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand(ERROR_UNDONE_NO_ID, WHERE_ID);
	}

	QList<int> ids = parseIdList(commandString);
	QList< QSharedPointer<ICommand> > commands;

	qSort(ids.begin(), ids.end());

	foreach(int id, ids) {
		QSharedPointer<ICommand> command = QSharedPointer<ICommand>(new DoneCommand(id-1, false));
		commands.push_back(command);
	}

	return new CompositeCommand(commands);
}

// does the show command. takes in a string from user input
void Interpreter::doShow(QString commandString) {
	commandString = removeBefore(commandString, COMMAND_SHOW);
	commandString = commandString.trimmed();

	if (commandString == KEYWORD_DONE) {
		QList<Task> results = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isDone();
		});
		Tasuke::instance().updateTaskWindow(results, TITLE_DONE);
	} else if (commandString == KEYWORD_UNDONE) {
		QList<Task> results = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return !task.isDone();
		});
		Tasuke::instance().updateTaskWindow(results, TITLE_UNDONE);
	} else if (commandString == KEYWORD_ONGOING) {
		QList<Task> results = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isOngoing();
		});
		Tasuke::instance().updateTaskWindow(results, TITLE_ONGOING);
	} else if (commandString == KEYWORD_OVERDUE) {
		QList<Task> results = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isOverdue();
		});
		Tasuke::instance().updateTaskWindow(results, TITLE_OVERDUE);
	} else if (commandString == KEYWORD_TODAY) {
		QList<Task> results = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isDueToday();
		});
		Tasuke::instance().updateTaskWindow(results, TITLE_TODAY);
	} else if (commandString == KEYWORD_TOMORROW) {
		QList<Task> results = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isDueTomorrow();
		});
		Tasuke::instance().updateTaskWindow(results, TITLE_TOMORROW);
	} else  if (commandString == KEYWORD_NIL || commandString == KEYWORD_ALL
		|| commandString == KEYWORD_EVERYTHING) {
		Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
	} else if (commandString.startsWith(DELIMITER_HASH) && !commandString.contains(" ")) {
		QString tag = commandString.remove(0,1);
		QList<Task> results = Tasuke::instance().getStorage().searchByTag(tag);
		Tasuke::instance().updateTaskWindow(results, DELIMITER_HASH + tag);
	} else {
		commandString = substituteForDescription(commandString);
		QList<Task> results = Tasuke::instance().getStorage().searchByDescription(commandString);
		Tasuke::instance().updateTaskWindow(results, "\"" + commandString + "\"");
	}

	Tasuke::instance().showTaskWindow();
}

// does the about command.
void Interpreter::doAbout() {
	Tasuke::instance().showAboutWindow();
}

// does the hide command.
void Interpreter::doHide() {
	Tasuke::instance().hideTaskWindow();
}

// does the undo command. takes in a string from user input.
// if dry is true, nothing is done. defaults to false
// throws ExceptionBadCommand if unable to parse
void Interpreter::doUndo(QString commandString, bool dry) {
	commandString = removeBefore(commandString, COMMAND_UNDO);
	commandString = commandString.trimmed();

	int times = 1;

	if (commandString == KEYWORD_MAX) {
		times = Tasuke::instance().undoSize();
	} else if (!commandString.isEmpty()) {
		bool ok = false;
		times = commandString.toInt(&ok);

		if (ok == false) {
			throw ExceptionBadCommand(ERROR_NOT_A_NUMBER(commandString), WHERE_TIMES);
		}
	}

	if (dry) {
		return;
	}

	for (int i=0; i<times; i++) {
		Tasuke::instance().undoCommand();
	}
}

// does the redo command. takes in a string from user input.
// if dry is true, nothing is done. defaults to false
// throws ExceptionBadCommand if unable to parse
void Interpreter::doRedo(QString commandString, bool dry) {
	commandString = removeBefore(commandString, COMMAND_REDO);
	commandString = commandString.trimmed();

	int times = 1;

	if (commandString == KEYWORD_MAX) {
		times = Tasuke::instance().redoSize();
	} else if (!commandString.isEmpty()) {
		bool ok = false;
		times = commandString.toInt(&ok);

		if (ok == false) {
			throw ExceptionBadCommand(ERROR_NOT_A_NUMBER(commandString), WHERE_TIMES);
		}
	}

	if (dry) {
		return;
	}

	for (int i=0; i<times; i++) {
		Tasuke::instance().redoCommand();
	}
}

// does the settings command.
void Interpreter::doSettings() {
	Tasuke::instance().showSettingsWindow();
}

// does the help command.
void Interpreter::doHelp() {
	Tasuke::instance().showTutorial();
}

// does the exit command.
void Interpreter::doExit() {
	QApplication::quit();
}

// try to parse the id from a string input
// throws ExceptionBadCommand if unable to parse
int Interpreter::parseId(QString idString) {
	idString = idString.trimmed();

	if (idString == KEYWORD_LAST) {
		if (last < 0) {
			throw ExceptionBadCommand(ERROR_NO_LAST, WHERE_ID);
		}
		return last;
	}

	bool ok = false;
	int id = idString.toInt(&ok);

	if (ok == false) {
		throw ExceptionBadCommand(ERROR_NO_ID, WHERE_ID);
	}

	int numTasks = Tasuke::instance().getStorage().totalTasks();

	if (id < 1 || id > numTasks) {
		throw ExceptionBadCommand(ERROR_ID_OUT_OF_RANGE(id, numTasks), WHERE_ID);
	}

	return id;
}

// try to parse an id list from a string input
// throws ExceptionBadCommand if unable to parse
QList<int> Interpreter::parseIdList(QString idListString) {
	idListString = idListString.trimmed();
	
	QList<int> results;
	
	if (idListString == KEYWORD_ALL) {
		int lastId = Tasuke::instance().getStorage().totalTasks();
		for (int i=1; i<=lastId; i++) {
			results.push_back(i);
		}
		return results;
	}

	QStringList idListParts = idListString.split(DELIMITER_COMMA);
	foreach(QString idListPart, idListParts) {
		results.append(parseIdRange(idListPart));
	}

	return results;
}

// try to parse an id range from a string input
// throws ExceptionBadCommand if unable to parse
QList<int> Interpreter::parseIdRange(QString idRangeString) {
	idRangeString = substituteForRange(idRangeString);
	idRangeString = idRangeString.trimmed();
	
	QList<int> results;
	QList<Task> special;
	if (idRangeString == KEYWORD_DONE) {
		special = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isDone();
		});
	} else if (idRangeString == KEYWORD_OVERDUE) {
		special = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isOverdue();
		});
	} else if (idRangeString == KEYWORD_TODAY) {
		special = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isDueToday();
		});
	} else if (idRangeString == KEYWORD_UNDONE) {
		special = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return !task.isDone();
		});
	}

	// if this is a sepcial range
	if (special.size() > 0) {
		foreach(Task task, special) {
			results.push_back(task.getId()+1);
		}

		return results;
	}

	QStringList idRangeParts = idRangeString.split(DELIMITER_DASH);

	if (idRangeParts.size() == 1) {
		results.push_back(parseId(idRangeParts[0]));
	} else if (idRangeParts.size() == 2) {
		int begin = parseId(idRangeParts[0]);
		int end = parseId(idRangeParts[1]);

		if (end < begin) {
			throw ExceptionBadCommand(ERROR_ID_INVALID_RANGE(begin, end), WHERE_ID);
		}

		for (int i=begin; i<=end; i++) {
			results.push_back(i);
		}
	} else {
		throw ExceptionBadCommand(ERROR_ID_NO_A_RANGE(idRangeString), WHERE_ID);
	}

	return results;
}

// try to parse the time period from a string input
// throws ExceptionBadCommand if unable to parse
Interpreter::TIME_PERIOD Interpreter::parseTimePeriod(QString timePeriodString) {
	timePeriodString = substituteForRange(timePeriodString);

	QStringList timePeriodParts = timePeriodString.split(DELIMITER_DASH);
	TIME_PERIOD timePeriod;

	if (timePeriodParts.size() > 2) {
		throw ExceptionBadCommand(ERROR_DATE_NO_A_RANGE(timePeriodString), WHERE_DATE);
	}

	if (timePeriodParts.size() == 1) {
		timePeriod.end = parseDate(timePeriodString);
	} else if (timePeriodParts.size() == 2) {
		timePeriod.begin = parseDate(timePeriodParts[0], false);
		timePeriod.end = parseDate(timePeriodParts[1]);

		if (!timePeriod.begin.isValid()) {
			throw ExceptionBadCommand(ERROR_DATE_BEGIN, WHERE_BEGIN);
		}
	}

	if (!timePeriod.end.isValid()) {
		throw ExceptionBadCommand(ERROR_DATE_END, WHERE_END);
	}

	if (timePeriod.begin.isValid() && timePeriod.end < timePeriod.begin) {
		throw ExceptionBadCommand(ERROR_DATE_INVALID_PERIOD(timePeriod), WHERE_DATE);
	}

	return timePeriod;
}

// try to parse the date from a string input
// throws ExceptionBadCommand if unable to parse
QDateTime Interpreter::parseDate(QString dateString, bool isEnd) {
	dateString = substituteForDate(dateString);
	dateString = dateString.trimmed();
	dateString = dateString.toLower();

	QDate currentDate = QDate::currentDate();
	QTime timePart = TIME_BEFORE_MIDNIGHT;
	if (!isEnd) {
		timePart = TIME_MIDNIGHT;
	}

	if (!formatsAlreadyInit) {
		initFormats();
	}

	QDateTime retVal;

	if (dateString.contains(TIME_AM) || dateString.contains(TIME_PM)) {
		// if the datetime contains am/pm means we can cut our search space

		// these formats need the date added
		foreach(QString timeFormat, timeFormatsAp) {
			QTime timePart = QTime::fromString(dateString, timeFormat);
			if (timePart.isValid()) {
				retVal.setDate(currentDate);
				retVal.setTime(timePart);
				return retVal;
			}
		}

		// these formats need the year added
		foreach(QString dateTimeFormat, dateTimeFormatsWithoutYearAp) {
			retVal = QDateTime::fromString(dateString, dateTimeFormat);
			if (retVal.isValid()) {
				QDate date = retVal.date();
				retVal.setDate(QDate(currentDate.year(), date.month(), date.day()));
				return retVal;
			}
		}

		// these formats are complete
		foreach(QString dateTimeFormat, dateTimeFormatsAp) {
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

		// the other formats do not include am/pm so probably invalid
		return retVal;
	}

	// these formats need the date added
	foreach(QString timeFormat, timeFormats) {
		QTime timePart = QTime::fromString(dateString, timeFormat);
		if (timePart.isValid()) {
			retVal.setDate(currentDate);
			retVal.setTime(timePart);
			return retVal;
		}
	}

	// these formats need the current year and time added
	foreach(QString dateFormat, dateFormatsWithoutYear) {
		retVal = QDateTime::fromString(dateString, dateFormat);
		if (retVal.isValid()) {
			QDate date = retVal.date();
			retVal.setDate(QDate(currentDate.year(), date.month(), date.day()));
			retVal.setTime(timePart);
			return retVal;
		}
	}
	
	// these formats need the year added
	foreach(QString dateTimeFormat, dateTimeFormatsWithoutYear) {
		retVal = QDateTime::fromString(dateString, dateTimeFormat);
		if (retVal.isValid()) {
			QDate date = retVal.date();
			retVal.setDate(QDate(currentDate.year(), date.month(), date.day()));
			return retVal;
		}
	}

	// these formats need the time added
	foreach(QString dateFormat, dateFormats) {
		retVal = QDateTime::fromString(dateString, dateFormat);
		if (retVal.isValid()) {
			QDate date = retVal.date();
			if (date.year() < YEARS_MILIENIUM) {
				// add a century
				date = date.addYears(YEARS_CENTURY);
				retVal.setDate(date);
			}
			retVal.setTime(timePart);
			return retVal;
		}
	}

	// these formats are complete
	foreach(QString dateTimeFormat, dateTimeFormats) {
		retVal = QDateTime::fromString(dateString, dateTimeFormat);
		if (retVal.isValid()) {
			QDate date = retVal.date();
			if (date.year() < YEARS_MILIENIUM) {
				// add a century
				date = date.addYears(YEARS_CENTURY);
				retVal.setDate(date);
			}
			return retVal;
		}
	}

	return retVal;
}

// generate all date formats
// WARNING: this method blocks for very long time.
// run this method on another thread at start up
// this method is threadsafe
void Interpreter::initFormats() {
	QMutexLocker locker(&mutex);

	if (formatsAlreadyInit) {
		return;
	}

	generateTimeFormats();
	generateDateFormatsWithoutYear();
	generateDateFormats();
	generateDateTimeFormatsWithoutYear();
	generateDateTimeFormats();

	formatsAlreadyInit = true;
}

// generate formats for time only
// do not call this directly. use initFormats() to run
void Interpreter::generateTimeFormats() {
	assert(formatsAlreadyInit == false);

	QStringList hourFormats;
	QStringList minuteFomats;
	QStringList amPmFormats;
	QStringList separators;
	QStringList optionalSpaces;
	
	hourFormats << "h" << "hh";
	minuteFomats << "mm";
	amPmFormats << "ap";
	separators << " " << ":" << ".";
	optionalSpaces << " " << "";

	QStringList toAdd;
	foreach(QString hourFormat, hourFormats) {
		foreach(QString minuteFormat, minuteFomats) {
			foreach(QString separator, separators) {
				timeFormats << (hourFormat + separator + minuteFormat);
			}
		}
	}
	foreach(QString timeFormat, timeFormats) {
		foreach(QString amPmFormat, amPmFormats) {
			foreach(QString optionalSpace, optionalSpaces) {
				timeFormatsAp << (timeFormat + optionalSpace + amPmFormat);
			}
		}
	}
	
	// special constructions
	// military time:
	timeFormats << "hhmm'hrs'";;
	// 5pm:
	foreach(QString hourFormat, hourFormats) {
		foreach(QString amPmFormat, amPmFormats) {
			foreach(QString optionalSpace, optionalSpaces) {
				timeFormatsAp << (hourFormat + optionalSpace + amPmFormat);
			}
		}
	}
}

// generates date formats without any year
// do not call this directly. use initFormats() to run
void Interpreter::generateDateFormatsWithoutYear() {
	assert(formatsAlreadyInit == false);

	QStringList dayFormats;
	QStringList speltMonthFormats;
	QStringList monthFormats;
	QStringList dateSeparators;

	dayFormats << "d" << "dd";
	speltMonthFormats << "MMM" << "MMMM";
	monthFormats << "M" << "MM";
	dateSeparators << "/" << "-"; 

	foreach(QString dayFormat, dayFormats) {
		foreach(QString speltMonthFormat, speltMonthFormats) {
			dateFormatsWithoutYear << (dayFormat + " " + speltMonthFormat);
			// american format:
			dateFormatsWithoutYear << (speltMonthFormat + " " + dayFormat);
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

// generates date formats with the years
// do not call this directly. use initFormats() to run
void Interpreter::generateDateFormats() {
	assert(formatsAlreadyInit == false);

	QStringList dayFormats;
	QStringList monthFormats;
	QStringList yearFormats;
	QStringList dateSeparators;

	dayFormats << "d" << "dd";
	monthFormats << "M" << "MM";
	yearFormats << "yy" << "yyyy";
	dateSeparators << "/" << "-"; 

	foreach(QString dateFormatWithoutYear, dateFormatsWithoutYear) {
		foreach(QString yearFormat, yearFormats) {
			dateFormats << (dateFormatWithoutYear + " " + yearFormat);
			dateFormats << (yearFormat + " " + dateFormatWithoutYear);
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

// generates date + time formats without any year
// do not call this directly. use initFormats() to run
void Interpreter::generateDateTimeFormatsWithoutYear() {
	assert(formatsAlreadyInit == false);

	foreach(QString dateFormatWithoutYear, dateFormatsWithoutYear) {
		foreach(QString timeFormat, timeFormats) {
			dateTimeFormatsWithoutYear << (dateFormatWithoutYear + " " + timeFormat);
			dateTimeFormatsWithoutYear << (timeFormat + " " + dateFormatWithoutYear);
		}

		foreach(QString timeFormatAp, timeFormatsAp) {
			dateTimeFormatsWithoutYearAp << (dateFormatWithoutYear + " " + timeFormatAp);
			dateTimeFormatsWithoutYearAp << (timeFormatAp + " " + dateFormatWithoutYear);
		}
	}
}

// generates date + time formats with the year
// do not call this directly. use initFormats() to run
void Interpreter::generateDateTimeFormats() {
	assert(formatsAlreadyInit == false);

	foreach(QString dateFormat, dateFormats) {
		foreach(QString timeFormat, timeFormats) {
			dateTimeFormats << (dateFormat + " " + timeFormat);
			dateTimeFormats << (timeFormat + " " + dateFormat);
		}

		foreach(QString timeFormatAp, timeFormatsAp) {
			dateTimeFormatsAp << (dateFormat + " " + timeFormatAp);
			dateTimeFormatsAp << (timeFormatAp + " " + dateFormat);
		}
	}
}