#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>
#include "Task.h"

// This class abstracts away the data management in memory and on disk.
// Usually only 1 instance of this class is required and it is managed by the
// Tasuke singleton.
class Storage {
private:
	std::vector<Task> tasks;
	
public:
	Storage();
	void loadFile();
	void saveFile();

	void addTask(Task& task);
	Task& getTask(int i);
	void removeTask(int i);
};

#endif
