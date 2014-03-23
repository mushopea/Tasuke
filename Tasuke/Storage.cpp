#include <glog/logging.h>
#include <QSettings>
#include <QVariant>
#include <QJsonArray>
#include <QStandardPaths>
#include <iostream>
#include "Constants.h"
#include "Exceptions.h"
#include "Storage.h"
#include "Tasuke.h"

IStorage::IStorage() {
	
}

IStorage::~IStorage() {

}

void IStorage::addTask(Task& task) {
	LOG(INFO) << "Adding task " << task.getDescription().toStdString();

	tasks.push_back(task);
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

void IStorage::addTask(Task& task, int pos) {
	LOG(INFO) << "Adding task " << task.getDescription().toStdString()
		<< " at position " << pos;

	tasks.insert(pos, task);
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

Task& IStorage::getTask(int pos) {
	return tasks[pos];
}

void IStorage::removeTask(int pos) {
	LOG(INFO) << "Removing task at position " << pos;

	tasks.removeAt(pos);
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

void IStorage::popTask() {
	LOG(INFO) << "Popping task from back";

	tasks.pop_back();
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

QList<Task> IStorage::getTasks() {
	return tasks;
}

int IStorage::totalTasks() {
	return tasks.size();
}

// Constructor for Storage.
Storage::Storage() {
	LOG(INFO) << "Storage instance created...";

	path = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/tasuke.ini";

	qRegisterMetaType<Task>("Task");
	qRegisterMetaTypeStreamOperators<Task>("Task");	
}

// This function loads the contents of the text file and serialize it into
// the memory. If there is no such file, this function does nothing.
void Storage::loadFile() {
	LOG(INFO) << "Loading file...";

	QSettings settings(path, QSettings::IniFormat);
	int size = settings.beginReadArray("tasks");
	for (int i=0; i<size; i++) {
		settings.setArrayIndex(i);
		Task task = settings.value("task").value<Task>();
		tasks.push_back(task);
	}
	settings.endArray();

	LOG(INFO) << "File loaded.";
}

// This function deserialize the data from memory and writes it to the text
// file. If the file cannot be written, an ExceptionNotOpen is thrown.
void Storage::saveFile() {
	LOG(INFO) << "Saving file";

	QSettings settings(path, QSettings::IniFormat);
	settings.beginWriteArray("tasks");
	for (int i=0; i<tasks.size(); i++) {
		settings.setArrayIndex(i);
		settings.setValue("task", QVariant::fromValue<Task>(tasks[i]));
	}
	settings.endArray();
}