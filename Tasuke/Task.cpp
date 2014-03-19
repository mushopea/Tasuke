#include <QDataStream>
#include <string>
#include "Task.h"

Task::Task() {
	done = false;
}

Task::~Task() {

}

void Task::setDescription(QString& _description) {
	description = _description;
}

QString Task::getDescription() const {
	return description;
}

void Task::addTag(QString& tag) {
	tags.push_back(tag);
}

void Task::removeTag(QString& tag) {
	tags.removeOne(tag);
}

QList<QString> Task::getTags() {
	return tags;
}

void Task::setBegin(QDateTime& _begin) {
	begin = _begin;
}

QDateTime Task::getBegin() const {
	return begin;
}

void Task::setEnd(QDateTime& _end) {
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

bool Task::isDone() const {
	return done;
}

QDataStream& operator<<(QDataStream& out, const Task& task) {
	out << task.description;
	out << task.tags.size();
	for (int i=0; i<task.tags.size(); i++) {
		out << task.tags[i];
	}
	out << task.begin;
	out << task.end;

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

	return in;
}