#include <QSettings>
#include <QVariant>
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