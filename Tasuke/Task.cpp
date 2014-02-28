#include "Task.h"

Task::Task() {

}

Task::~Task() {

}

void Task::setDescription(std::string& _description) {
	description = _description;
}

std::string Task::getDescription() const {
	return description;
}

void Task::addTag(std::string& tag) {
	tags.push_back(tag);
}

void Task::removeTag(std::string& tag) {
	tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());
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