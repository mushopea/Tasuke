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

//@author A0096836M

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

QMutex Interpreter::mutex;

void Interpreter::setLast(int _last) {
	last = _last;
}

QString Interpreter::substitute(QString text) {
	QString subbedText = text;
	subbedText = subbedText.replace(" by ", " @ ");
	subbedText = subbedText.replace(" at ", " @ ");
	subbedText = subbedText.replace(" from ", " @ ");
	subbedText = subbedText.replace(" on ", " @ ");

	subbedText = subbedText.replace(" \\by ", " by ");
	subbedText = subbedText.replace(" \\at ", " at ");
	subbedText = subbedText.replace(" \\from ", " from ");
	subbedText = subbedText.replace(" \\on ", " on ");
	subbedText = subbedText.replace(" \\to ", " to ");

	subbedText = subbedText.replace(QRegExp("^do "), "add ");
	subbedText = subbedText.replace(QRegExp("^create"), "add");
	subbedText = subbedText.replace(QRegExp("^change"), "edit");
	subbedText = subbedText.replace(QRegExp("^rm"), "remove");
	subbedText = subbedText.replace(QRegExp("^delete"), "remove");
	subbedText = subbedText.replace(QRegExp("^ls"), "show");
	subbedText = subbedText.replace(QRegExp("^quit"), "exit");
	subbedText = subbedText.replace(QRegExp("^search"), "show");
	subbedText = subbedText.replace(QRegExp("^find"), "show");

	return subbedText;
}

QString Interpreter::substituteForTimePeriod(QString text) {
	QString subbedText = text;
	subbedText = subbedText.replace(" to ", " - ");
	subbedText = subbedText.replace(" \\to ", " to ");

	return subbedText;
}

QDate Interpreter::nextWeekday(int weekday) {
	QDate date = QDate::currentDate();
	
	while (date.dayOfWeek() != weekday) {
		date = date.addDays(1);
	}

	return date;
}

QString Interpreter::substituteForDate(QString text) {
	QString subbedText = text.toLower();

	// strip commas
	subbedText = subbedText.replace(QRegExp(",\\b"), "");

	// strip this, next and the
	subbedText = subbedText.replace(QRegExp("\\bthis\\b"), "");
	subbedText = subbedText.replace(QRegExp("\\bnext\\b"), "");
	subbedText = subbedText.replace(QRegExp("\\bnthe\\b"), "");

	// expand out abbrev
	subbedText = subbedText.replace(QRegExp("\\b2day\\b"), "today");
	subbedText = subbedText.replace(QRegExp("\\btmr\\b"), "tomorrow");
	subbedText = subbedText.replace(QRegExp("\\btml\\b"), "tomorrow");
	subbedText = subbedText.replace(QRegExp("\\bmon\\b"), "monday");
	subbedText = subbedText.replace(QRegExp("\\btue\\b"), "tuesday");
	subbedText = subbedText.replace(QRegExp("\\btues\\b"), "tuesday");
	subbedText = subbedText.replace(QRegExp("\\bwed\\b"), "wednesday");
	subbedText = subbedText.replace(QRegExp("\\bthu\\b"), "thursday");
	subbedText = subbedText.replace(QRegExp("\\bthur\\b"), "thursday");
	subbedText = subbedText.replace(QRegExp("\\bthurs\\b"), "thursday");
	subbedText = subbedText.replace(QRegExp("\\bfri\\b"), "friday");
	subbedText = subbedText.replace(QRegExp("\\bsat\\b"), "saturday");
	subbedText = subbedText.replace(QRegExp("\\bsun\\b"), "sunday");

	// today, tomorrow etc.
	subbedText = subbedText.replace(QRegExp("\\byesterday\\b"), QDate::currentDate().addDays(-1).toString("dd/MM/yyyy"));
	subbedText = subbedText.replace(QRegExp("\\btoday\\b"), QDate::currentDate().toString("dd/MM/yyyy"));
	subbedText = subbedText.replace(QRegExp("\\btomorrow\\b"), QDate::currentDate().addDays(1).toString("dd/MM/yyyy"));
	subbedText = subbedText.replace(QRegExp("\\bday after tomorrow\\b"), QDate::currentDate().addDays(2).toString("dd/MM/yyyy"));

	// weekdays
	if (subbedText.contains("monday")) {
		subbedText = subbedText.replace("monday", nextWeekday(1).toString("dd/MM/yyyy"));
	} else if (subbedText.contains("tuesday")) {
		subbedText = subbedText.replace("tuesday", nextWeekday(2).toString("dd/MM/yyyy"));
	} else if (subbedText.contains("wednesday")) {
		subbedText = subbedText.replace("wednesday", nextWeekday(3).toString("dd/MM/yyyy"));
	} else if (subbedText.contains("thursday")) {
		subbedText = subbedText.replace("thursday", nextWeekday(4).toString("dd/MM/yyyy"));
	} else if (subbedText.contains("friday")) {
		subbedText = subbedText.replace("friday", nextWeekday(5).toString("dd/MM/yyyy"));
	} else if (subbedText.contains("saturday")) {
		subbedText = subbedText.replace("saturday", nextWeekday(6).toString("dd/MM/yyyy"));
	} else if (subbedText.contains("sunday")) {
		subbedText = subbedText.replace("sunday", nextWeekday(7).toString("dd/MM/yyyy"));
	}

	// time of days
	subbedText = subbedText.replace(QRegExp("\\bdawn\\b"), QTime(6,0).toString("hh:mm ap"));
	subbedText = subbedText.replace(QRegExp("\\bmorning\\b"), QTime(10,0).toString("hh:mm ap"));
	subbedText = subbedText.replace(QRegExp("\\bnoon\\b"), QTime(12,0).toString("hh:mm ap"));
	subbedText = subbedText.replace(QRegExp("\\bafternoon\\b"), QTime(18,0).toString("hh:mm ap"));
	subbedText = subbedText.replace(QRegExp("\\bevening\\b"), QTime(20,0).toString("hh:mm ap"));
	subbedText = subbedText.replace(QRegExp("\\bnight\\b"), QTime(22,0).toString("hh:mm ap"));
	subbedText = subbedText.replace(QRegExp("\\bmidnight\\b"), QTime(23,59).toString("hh:mm ap"));

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
			expectNewDelimiter = false;

			if (tokens[i][0] == '@') {
				current = tokens[i][0];
				tokens[i].remove(0,1);

				if (retVal.contains(current) == true) {
					throw ExceptionBadCommand("You can't have more than 2 time periods or deadlines in a task.", "date");
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
				throw ExceptionBadCommand(QString("I don't know what to do for '%1'").arg(tokens[i]), "description");
			}
		}

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

QString Interpreter::getType(QString commandString, bool doSub) {
	if (doSub) {
		commandString = substitute(commandString);
	}

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
ICommand* Interpreter::interpret(QString commandString, bool dry) {
	LOG(INFO) << "Interpretting " << commandString.toStdString();

	commandString = substitute(commandString);

	QString commandType = getType(commandString, false);

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
	} else if (commandType == "clear") {
		return createClearCommand(commandString);
	}

	if (commandType == "") {
		throw ExceptionBadCommand("I don't undesrtand this command.");
	}

	if (commandType == "undo") {
		doUndo(commandString, dry);
	} else if (commandType == "redo") {
		doRedo(commandString, dry);
	}
	
	// if this was a dry run, don't actually do anything
	if (dry) {
		return nullptr;
	}

	if (commandType == "show") {
		doShow(commandString);
	} else if (commandType == "hide") {
		doHide();
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

ICommand* Interpreter::createAddCommand(QString commandString) {
	commandString = removeBefore(commandString, "add");
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand("You need to give a description to your task.", "description");
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

ICommand* Interpreter::createRemoveCommand(QString commandString) {
	commandString = removeBefore(commandString, "remove");
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand("You must tell me which task(s) to remove.", "id");
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

ICommand* Interpreter::createEditCommand(QString commandString) {
	commandString = removeBefore(commandString, "edit");
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand("You must tell me which task to edit.", "id");
	}

	QString idString = commandString.split(' ')[0];
	int id = parseId(idString);

	commandString = commandString.section(' ', 1);

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand("You must tell me what you want to change for the task.", "description");
	}

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

ICommand* Interpreter::createClearCommand(QString commandString) {
	return new ClearCommand();
}

ICommand* Interpreter::createDoneCommand(QString commandString) {
	commandString = removeBefore(commandString, "done");
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand("You must tell me what to mark as done.", "id");
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

ICommand* Interpreter::createUndoneCommand(QString commandString) {
	commandString = removeBefore(commandString, "undone");
	commandString = commandString.trimmed();

	if (commandString.isEmpty()) {
		throw ExceptionBadCommand("You must tell me what to mark as undone.", "id");
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

void Interpreter::doShow(QString commandString) {
	commandString = removeBefore(commandString, "show");
	commandString = commandString.trimmed();

	if (commandString == "done") {
		QList<Task> results = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isDone();
		});
		Tasuke::instance().updateTaskWindow(results, "done");
	} else if (commandString == "undone") {
		QList<Task> results = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isDone();
		});
		Tasuke::instance().updateTaskWindow(results, "undone");
	} else if (commandString == "ongoing") {
		QList<Task> results = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isOngoing();
		});
		Tasuke::instance().updateTaskWindow(results, "ongoing");
	} else if (commandString == "overdue") {
		QList<Task> results = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isOverdue();
		});
		Tasuke::instance().updateTaskWindow(results, "overdue");
	} else if (commandString == "today") {
		QList<Task> results = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isDueToday();
		});
		Tasuke::instance().updateTaskWindow(results, "due today");
	} else if (commandString == "tomorrow") {
		QList<Task> results = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isDueTomorrow();
		});
		Tasuke::instance().updateTaskWindow(results, "due tomorrow");
	} else  if (commandString == "" || commandString == "all" || commandString == "everything") {
		Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
	} else if (commandString.startsWith("#") && !commandString.contains(" ")) {
		QString tag = commandString.remove(0,1);
		QList<Task> results = Tasuke::instance().getStorage().searchByTag(tag);
		Tasuke::instance().updateTaskWindow(results, "#" + tag);
	} else {
		QList<Task> results = Tasuke::instance().getStorage().searchByDescription(commandString);
		Tasuke::instance().updateTaskWindow(results, "\"" + commandString + "\"");
	}

	Tasuke::instance().showTaskWindow();
}
void Interpreter::doAbout() {
	Tasuke::instance().showAboutWindow();
}
void Interpreter::doHide() {
	Tasuke::instance().hideTaskWindow();
}
void Interpreter::doUndo(QString commandString, bool dry) {
	commandString = removeBefore(commandString, "undo");
	commandString = commandString.trimmed();

	int times = 1;

	if (commandString == "max") {
		times = Tasuke::instance().undoSize();
	} else if (!commandString.isEmpty()) {
		bool ok = false;
		times = commandString.toInt(&ok);

		if (ok == false) {
			throw ExceptionBadCommand(QString("'%1' doesn't look like a number").arg(commandString), "times");
		}
	}

	if (dry) {
		return;
	}

	for (int i=0; i<times; i++) {
		Tasuke::instance().undoCommand();
	}
}
void Interpreter::doRedo(QString commandString, bool dry) {
	commandString = removeBefore(commandString, "redo");
	commandString = commandString.trimmed();

	int times = 1;

	if (commandString == "max") {
		times = Tasuke::instance().redoSize();
	} else if (!commandString.isEmpty()) {
		bool ok = false;
		times = commandString.toInt(&ok);

		if (ok == false) {
			throw ExceptionBadCommand(QString("'%1' doesn't look like a number").arg(commandString), "times");
		}
	}

	if (dry) {
		return;
	}

	for (int i=0; i<times; i++) {
		Tasuke::instance().redoCommand();
	}
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
	idString = idString.trimmed();

	if (idString == "last") {
		if (last < 0) {
			throw ExceptionBadCommand("There is no last task", "id");
		}
		return last;
	}

	bool ok = false;
	int id = idString.toInt(&ok);

	if (ok == false) {
		throw ExceptionBadCommand(QString("You need to give me a task number").arg(idString), "id");
	}

	int numTasks = Tasuke::instance().getStorage().totalTasks();

	if (id < 1 || id > numTasks) {
		throw ExceptionBadCommand(QString("There is no task '%1'. Please give a number between 1 and %2").arg(QString::number(id), QString::number(numTasks)), "id");
	}

	return id;
}

QList<int> Interpreter::parseIdList(QString idListString) {
	idListString = idListString.trimmed();
	
	QList<int> results;
	
	if (idListString == "all") {
		int lastId = Tasuke::instance().getStorage().totalTasks();
		for (int i=1; i<=lastId; i++) {
			results.push_back(i);
		}
		return results;
	}

	QStringList idListParts = idListString.split(",");
	foreach(QString idListPart, idListParts) {
		results.append(parseIdRange(idListPart));
	}

	return results;
}

QList<int> Interpreter::parseIdRange(QString idRangeString) {
	idRangeString = idRangeString.trimmed();

	QList<int> results;
	QList<Task> special;
	if (idRangeString == "done") {
		special = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isDone();
		});
	} else if (idRangeString == "overdue") {
		special = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isOverdue();
		});
	} else if (idRangeString == "today") {
		special = Tasuke::instance().getStorage().search([](Task task) -> bool {
			return task.isDueToday();
		});
	} else if (idRangeString == "undone") {
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

	QStringList idRangeParts = idRangeString.split("-");

	if (idRangeParts.size() == 1) {
		results.push_back(parseId(idRangeParts[0]));
	} else if (idRangeParts.size() == 2) {
		int begin = parseId(idRangeParts[0]);
		int end = parseId(idRangeParts[1]);

		if (end < begin) {
			throw ExceptionBadCommand(QString("Start number (%1) is after end number (%2)").arg(begin, end), "id");
		}

		for (int i=begin; i<=end; i++) {
			results.push_back(i);
		}
	} else {
		throw ExceptionBadCommand(QString("'%1' doesn't look like a valid range").arg(idRangeString), "id");
	}

	return results;
}

Interpreter::TIME_PERIOD Interpreter::parseTimePeriod(QString timePeriodString) {
	timePeriodString = substituteForTimePeriod(timePeriodString);

	QStringList timePeriodParts = timePeriodString.split("-");
	TIME_PERIOD timePeriod;

	if (timePeriodParts.size() > 2) {
		throw ExceptionBadCommand(QString("'%1' doesn't look like a valid date range").arg(timePeriodString), "date");
	}

	if (timePeriodParts.size() == 1) {
		timePeriod.end = parseDate(timePeriodString);
	} else if (timePeriodParts.size() == 2) {
		timePeriod.begin = parseDate(timePeriodParts[0], false);
		timePeriod.end = parseDate(timePeriodParts[1]);

		if (!timePeriod.begin.isValid()) {
			throw ExceptionBadCommand("The start time provided doesn't seem to be a date/time", "begin");
		}
	}

	if (!timePeriod.end.isValid()) {
		throw ExceptionBadCommand("The deadline provided doesn't seem to be a date/time", "end");
	}

	if (timePeriod.begin.isValid() && timePeriod.end < timePeriod.begin) {
		throw ExceptionBadCommand(QString("Start time (%1) is after end time (%2)").arg(timePeriod.begin.toString(), timePeriod.end.toString()), "date");
	}

	return timePeriod;
}

QDateTime Interpreter::parseDate(QString dateString, bool isEnd) {
	dateString = substituteForDate(dateString);
	dateString = dateString.trimmed();

	QDate currentDate = QDate::currentDate();
	QTime timePart = QTime(23,59);
	if (!isEnd) {
		timePart = QTime(0,0);
	}

	if (!formatsAlreadyInit) {
		initFormats();
	}

	QDateTime retVal;

	if (dateString.contains("am") || dateString.contains("pm") || dateString.contains("AM") || dateString.contains("PM")) {
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
			if (date.year() < 2000) {
				// add a century
				date = date.addYears(100);
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
			if (date.year() < 2000) {
				// add a century
				date = date.addYears(100);
				retVal.setDate(date);
			}
			return retVal;
		}
	}

	return retVal;
}

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