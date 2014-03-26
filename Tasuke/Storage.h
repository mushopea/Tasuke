#ifndef STORAGE_H
#define STORAGE_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include "Task.h"

// Interface class for Storage.
class IStorage {
protected:
	QList<Task> tasks;
	QMutex mutex;
public:
	IStorage();
	virtual ~IStorage();

	void addTask(Task& task);
	void addTask(Task& task, int pos);
	Task& getTask(int pos);
	void removeTask(int pos);
	void popTask();
	QList<Task> getTasks();
	int totalTasks();

	virtual void loadFile() = 0;
	virtual void saveFile() = 0;
};

// This class abstracts away the data management in memory and on disk.
// Usually only 1 instance of this class is required and it is managed by the
// Tasuke singleton.
class Storage : public IStorage {
private:
	void sortByEndDate();
	void sortByBeginDate();
	void sortByDescription();
	void sortByDone();

public:
	Storage();
	void loadFile();
	void saveFile();

	void clearAllDone();
	void clearAllTasks();
};

#endif
