#include <QDataStream>
#include <string>
#include "Constants.h"
#include "Exceptions.h"
#include "Task.h"

Task::Task() {
	done = false;
}

Task::Task(QString description) {
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

// Returns the QString representation of a countdown from now to the end of a task.
// This method also returns a countup from the end of an overdue task to now.
// This method does NOT guarantee accuracy of dates. This method assumes that
// there are 12 identical months in a year, and there are exactly 4 weeks every month,
// and each month has exactly 30 days.
QString Task::getTimeDifferenceString() const {
	long delta = getEnd().toMSecsSinceEpoch() - QDateTime::currentDateTime().toMSecsSinceEpoch();
	bool isNegative = delta < 0;

	// If date is in the past, set it to positive.
	if (isNegative) {
		delta *= -1;
	}

	delta /= MILLISECONDS_IN_SECOND;			// Get rid of milliseconds.

	int seconds = delta % SECONDS_IN_MINUTE;	// Obtain number of seconds.
	delta /= SECONDS_IN_MINUTE;					// Get rid of seconds.

	int minutes = delta % MINUTES_IN_HOUR;		// Obtain number of minutes.
	delta /= MINUTES_IN_HOUR;					// Get rid of minutes.

	int hours = delta % HOURS_IN_DAY;			// Obtain number of hours.
	delta /= HOURS_IN_DAY;						// Get rid of hours.

	int days = delta % DAYS_IN_WEEK;			// Obtain number of days.
	delta /= DAYS_IN_WEEK;						// Get rid of days.

	int weeks = delta % WEEKS_IN_MONTH;			// Obtain number of weeks.
	delta /= WEEKS_IN_MONTH;					// Get rid of weeks.

	int months = delta % MONTHS_IN_YEAR;		// Obtain number of months.
	delta /= MONTHS_IN_YEAR;					// Get rid of months.

	int years = delta;							// Obtain number of years.

	QString result = "";
	if (years > 0) {
		if (years == 1) {
			result.append("1 year");
		} else { // Plural
			result.append("%1 years").arg(years);
		}
	}

	if (months > 0) {
		// Prepend a comma if there is a year in front.
		if (years > 0) {
			result.append(", ");
		}
		if (months == 1) {
			result.append("1 month");
		} else { // Plural
			result.append("%1 months").arg(months);
		}
	}

	if (weeks > 0) {
		// Prepend a comma if there is a month in front.
		if (months > 0) {
			result.append(", ");
		}
		if (weeks == 1) {
			result.append("1 week");
		} else { // Plural
			result.append("%1 weeks").arg(weeks);
		}
	}

	if (days > 0) {
		// Prepend a comma if there is a week in front.
		if (weeks > 0) {
			result.append(", ");
		}
		if (days == 1) {
			result.append("1 day");
		} else { //  Plural
			result.append("%1 days").arg(days);
		}
	}

	if (hours > 0) {
		// Prepend a comma if there is a day in front.
		if (days > 0) {
			result.append(", ");
		}
		if (hours == 1) {
			result.append("1 hour");
		} else { // Plural
			result.append("%1 hours").arg(hours);
		}
	}

	if (minutes > 0) {
		// Prepend a comma if there is an hour in front.
		if (hours > 0) {
			result.append(", ");
		}
		if (minutes == 1) {
			result.append("1 minute");
		} else { // Plural
			result.append("%1 minutes").arg(minutes);
		}
	}

	// Finally, add closing statement.
	if (isNegative) {
		result.append(" ago.");
	} else {
		result.append(" from now.");
	}

	return result;
}

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