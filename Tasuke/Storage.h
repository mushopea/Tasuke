#ifndef STORAGE_H
#define STORAGE_H

#include <QString>
#include <QList>
#include "Task.h"

// This class abstracts away the data management in memory and on disk.
// Usually only 1 instance of this class is required and it is managed by the
// Tasuke singleton.
class Storage {
private:
	QList<Task> tasks;
	
public:
	Storage();
	void loadFile();
	void saveFile();

	void addTask(Task& task);
	Task& getTask(int i);
	void removeTask(int i);
	void popTask();
};

#endif
