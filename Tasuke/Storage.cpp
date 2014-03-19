#include <QSettings>
#include <QVariant>
#include <qjsonarray.h>
#include <iostream>
#include "Constants.h"
#include "Exceptions.h"
#include "Storage.h"
#include "Tasuke.h"

// Constructor for Storage.
Storage::Storage() {
	qRegisterMetaType<Task>("Task");
	qRegisterMetaTypeStreamOperators<Task>("Task");	
}

// This function loads the contents of the text file and serialize it into
// the memory. If there is no such file, this function does nothing.
void Storage::loadFile() {
	QSettings settings("Tasuke","Tasuke");
	int size = settings.beginReadArray("tasks");
	for (int i=0; i<size; i++) {
		settings.setArrayIndex(i);
		Task task = settings.value("task").value<Task>();
		tasks.push_back(task);
	}
	settings.endArray();
}

// This function deserialize the data from memory and writes it to the text
// file. If the file cannot be written, an ExceptionNotOpen is thrown.
void Storage::saveFile() {
	QSettings settings("Tasuke","Tasuke");

	settings.beginWriteArray("tasks");
	for (int i=0; i<tasks.size(); i++) {
		settings.setArrayIndex(i);
		settings.setValue("task", QVariant::fromValue<Task>(tasks[i]));
	}
	settings.endArray();
}

// This function takes a Task object and transforms it into a QJsonObject.
// This is a convenience function meant to abstract away the process of
// unwrapping a Task object and wrapping the data into a QJsonObject.
// This method is mainly called by saveFile in order to take a list of
// tasks and turn it into a human-readable Json document.
//
// @author:a0096863
QJsonObject Storage::fromTaskToJson(Task& task) {
	QJsonObject qjs;
	const QString DATE_TIME_STRING = "dd MM yyyy HH mm ss";

	if (!task.getDescription().isNull()) {
		QJsonValue description(task.getDescription());
		qjs.insert("Description", description);
	}

	if (!task.getBegin().isNull()) {
		QString beginTime = task.getBegin().toString(DATE_TIME_STRING);
		QJsonValue begin(beginTime);
		qjs.insert("Begin", begin);
	}

	if (!task.getEnd().isNull()) {
		QString endTime = task.getEnd().toString(DATE_TIME_STRING);
		QJsonValue end(endTime);
		qjs.insert("End", end);
	}

	if (!task.getTags().isEmpty()) {
		QJsonArray array;
		for (QString& tag : task.getTags()) {
			QJsonValue jsTag(tag);
			array.append(jsTag);
		}
		QJsonValue tagList(array);
		qjs.insert("Tags", tagList);
	}

	QJsonValue done(task.isDone());
	qjs.insert("Done", done);

	return qjs;
}

// This function takes a JsonObject and attempts to convert it into a task
// object. It is a convenience function meant to abstract away the task of
// unwrapping a Json object and wrapping the data elements into a Task
// object, which is then returned to the calling function.
//
// @author:a0096863
Task Storage::fromJsonToTask(QJsonObject& json) {
	Task task;
	const QString DATE_TIME_STRING = "dd MM yyyy HH mm ss";
	
	QJsonValue description = json.value("Description");
	if (!description.isUndefined()) {
		task.setDescription(description.toString());
	}

	QJsonValue beginTime = json.value("Begin");
	if (!beginTime.isUndefined()) {
		QString beginString = beginTime.toString();
		QDateTime begin	= QDateTime::fromString(beginString, DATE_TIME_STRING);
		task.setBegin(begin);
	}

	QJsonValue endTime = json.value("End");
	if (!endTime.isUndefined()) {
		QString endString = endTime.toString();
		QDateTime end = QDateTime::fromString(endString, DATE_TIME_STRING);
		task.setEnd(end);
	}

	QJsonValue tags = json.value("Tags");
	if (!tags.isUndefined()) {
		QJsonArray tagList = tags.toArray();
		for (QJsonValue tag : tagList) {
			QString t = tag.toString();
			task.addTag(t);
		}
	}

	bool done = json.value("Done").toBool();
	task.setDone(done);

	return task;
}

void Storage::addTask(Task& task) {
	tasks.push_back(task);
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

void Storage::addTask(Task& task, int pos) {
	tasks.insert(pos, task);
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

Task& Storage::getTask(int pos) {
	return tasks[pos];
}

void Storage::removeTask(int pos) {
	tasks.removeAt(pos);
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

void Storage::popTask() {
	tasks.pop_back();
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

QList<Task> Storage::getTasks() {
	return tasks;
}

int Storage::totalTasks() {
	return tasks.size();
}