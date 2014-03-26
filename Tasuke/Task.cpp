#include <QDataStream>
#include <string>
#include "Task.h"

Task::Task() {
	done = false;
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
	tags.push_back(tag);
}

void Task::removeTag(QString tag) {
	tags.removeOne(tag);
}

QList<QString> Task::getTags() {
	return tags;
}

void Task::setBegin(QDateTime _begin) {
	begin = _begin;
}

QDateTime Task::getBegin() const {
	return begin;
}

void Task::setEnd(QDateTime _end) {
	end = _end;
}

QDateTime Task::getEnd() const {
	return end;
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

// Returns FALSE if there is no end date/time for this task,
// or it is not valid.
// Returns FALSE if end date/time for this task is later
// than current date/time.
// Returns TRUE if end date/time for this task is earlier
// than current date/time.
bool Task::isOverdue() {
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
bool Task::isOngoing() {
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