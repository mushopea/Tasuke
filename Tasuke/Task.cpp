#include <QDataStream>
#include <string>
#include "Constants.h"
#include "Exceptions.h"
#include "Task.h"

Task::Task() {
	id = -1;
	done = false;
}

Task::Task(QString description) {
	id = -1;
	done = false;
	this->description = description;
}

Task::~Task() {

}

void Task::setDescription(QString _description) {
	description = _description;
}

QString Task::getDescription() const {
	return description;
}

void Task::addTag(QString tag) {
	if (tags.size() >= MAXIMUM_TAGS) {
		throw ExceptionTooManyTags();
	}

	tags.push_back(tag);
}

void Task::removeTag(QString tag) {
	tags.removeOne(tag);
}

QList<QString> Task::getTags() const {
	return tags;
}

void Task::setBegin(QDateTime _begin) {
	begin = _begin;
	beginDate = _begin.date();
	beginTime = _begin.time();
}

void Task::setBeginDate(QDate _beginDate) {
	beginDate = _beginDate;
	begin.setDate(_beginDate);
}

void Task::setBeginTime(QTime _beginTime) {
	beginTime = _beginTime;
	begin.setTime(_beginTime);
}

QDateTime Task::getBegin() const {
	return begin;
}

void Task::setEnd(QDateTime _end) {
	end = _end;
	endDate = _end.date();
	endTime = _end.time();
}

void Task::setEndDate(QDate _endDate) {
	endDate = _endDate;
	end.setDate(_endDate);
}

void Task::setEndTime(QTime _endTime) {
	endTime = _endTime;
	end.setTime(_endTime);
}

QDateTime Task::getEnd() const {
	return end;
}
/*
// Possibly the biggest piece of code in this object.
QString Task::getTimeLeftString() const {
	
}*/

void Task::setDone(bool _done) {
	done = _done;
}

void Task::markDone() {
	done = true;
}

void Task::markUndone() {
	done = false;
}

bool Task::isDone() const {
	return done;
}

void Task::setId(int identifier) {
	id = identifier;
}

int Task::getId() const {
	return id;
}

// Returns TRUE if task has neither a valid begin date/time, 
// nor a valid end date/time.
// Returns FALSE for all other cases.
bool Task::isFloating() const {
	if (!begin.isValid() && !end.isValid()) {
		return true;
	} else {
		return false;
	}
}

// Returns FALSE if there is no end date/time for this task,
// or it is not valid.
// Returns FALSE if end date/time for this task is later
// than current date/time.
// Returns TRUE if end date/time for this task is earlier
// than current date/time.
bool Task::isOverdue() const {
	if (end.isNull() || !end.isValid()) {
		return false;
	}
	if (end < QDateTime::currentDateTime()) {
		return true;
	} else {
		return false;
	}
}

// Returns FALSE if there is no begin date/time for this task,
// or it is not valid.
// Returns FALSE if start date/time for this task is later
// than current date/time.
// Returns FALSE if task is already overdue.
// Returns TRUE if start date/time for this task is earlier
// than current date.time.
bool Task::isOngoing() const {
	if (begin.isNull() || !begin.isValid()) {
		return false;
	}
	if (isOverdue()) {
		return false;
	}
	if (begin < QDateTime::currentDateTime()) {
		return true;
	} else {
		return false;
	}
}

// Returns FALSE if task has no valid end date.
// Returns TRUE if task is already overdue.
// Returns TRUE if task has an end time within specified date.
bool Task::isDueOn(QDate _date) const {
	QDateTime dateStart(_date, QTime(0, 0, 0));
	QDateTime dateEnd(_date, QTime(23, 59, 59));

	if (!getEnd().isValid()) {
		return false;
	}

	if (isOverdue()) {
		return true;
	}

	if ((getEnd() <= dateEnd) && (getEnd() >= dateStart)) {
		return true;
	} else {
		return false;
	}
}

// Returns FALSE if this task has no valid end date.
// Returns FALSE if this task is already overdue.
// Returns FALSE if this task has a due date that is not
// the current day, 2359hrs.
// Returns TRUE if this task is already overdue but the due date
// is the current day.
// Returns TRUE if this task has a due date that is
// the current day, 2359hrs.
bool Task::isDueToday() const {
	QDateTime todayStart(QDateTime::currentDateTime().date(), QTime(0, 0, 0));
	QDateTime todayEnd(QDateTime::currentDateTime().date(), QTime(23, 59, 59));

	if (!getEnd().isValid()) {
		return false;
	}

	if (isOverdue()) {
		if (getEnd() >= todayStart) {
			return true;
		} else {
			return false;
		}
	}

	if (getEnd() <= todayEnd) {
		return true;
	} else {
		return false;
	}
}

bool Task::isDueTomorrow() const {
	QDateTime tomorrowStart(QDateTime::currentDateTime().date().addDays(1), QTime(0, 0, 0));
	QDateTime tomorrowEnd(QDateTime::currentDateTime().date().addDays(1), QTime(23, 59, 59));

	if (!getEnd().isValid()) {
		return false;
	}

	if (isOverdue()) {
		if (getEnd() >= tomorrowStart) {
			return true;
		} else {
			return false;
		}
	}

	if (getEnd() <= tomorrowEnd) {
		return true;
	} else {
		return false;
	}
}

// An event is defined as a task that has a begin and end date/time.
// This method returns true if a task has both, and false if otherwise.
bool Task::isEvent() const {
	if (begin.isValid() && end.isValid()) {
		return true;
	} else {
		return false;
	}
}

bool Task::operator!=(const Task& other) const {
	return true;
}

bool Task::operator==(const Task& other) const {
	return true;
}
/*
bool Task::operator<(const Task& other) const {
	return true;
}

bool Task::operator<=(const Task& other) const {
	return true;
}

bool Task::operator>(const Task& other) const {
	return true;
}

bool Task::operator>=(const Task& other) const {
	return true;
}
*/

QDataStream& operator<<(QDataStream& out, const Task& task) {
	out << task.description;
	out << task.tags.size();
	for (int i=0; i<task.tags.size(); i++) {
		out << task.tags[i];
	}
	out << task.begin;
	out << task.end;
	out << task.done;

	return out;
}

QDataStream& operator>>(QDataStream& in, Task& task) {
	in >> task.description;
	int numTags = 0;
	in >> numTags;
	for (int i=0; i<numTags; i++) {
		QString tag;
		in >> tag;
		task.tags.push_back(tag);
	}
	in >> task.begin;
	in >> task.end;
	in >> task.done;

	return in;
}