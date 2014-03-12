#ifndef STORAGE_H
#define STORAGE_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include "Task.h"

// This class abstracts away the data management in memory and on disk.
// Usually only 1 instance of this class is required and it is managed by the
// Tasuke singleton.
class Storage {
private:
	QList<Task> tasks;
	QJsonObject fromTaskToJson(Task& task);
	Task fromJsonToTask(QJsonObject& jsonLine);
	
public:
	Storage();
	void loadFile();
	void saveFile();

	void addTask(Task& task);
	void addTask(Task& task, int pos);
	Task& getTask(int pos);
	void removeTask(int pos);
	void popTask();
	QList<Task> getTasks();
	int totalTasks();
};

#endif
