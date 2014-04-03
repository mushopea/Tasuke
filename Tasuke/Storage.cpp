#define NOMINMAX

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
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Adding task " << task.getDescription().toStdString();

	tasks.push_back(task);
	saveFile();
	QString description = task.getDescription();
	int id = 0;
	foreach(Task task, tasks) {
		if (task.getDescription() == description) {
			id = task.getId();
			break;
		}
	}
	Tasuke::instance().updateTaskWindow(tasks);
	Tasuke::instance().highightTask(id);
}

void IStorage::addTask(Task& task, int pos) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Adding task " << task.getDescription().toStdString()
		<< " at position " << pos;

	tasks.insert(pos, task);
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

Task& IStorage::getTask(int pos) {
	QMutexLocker lock(&mutex);
	return tasks[pos];
}

void IStorage::removeTask(int pos) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Removing task at position " << pos;

	tasks.removeAt(pos);
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

void IStorage::popTask() {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Popping task from back";

	tasks.pop_back();
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

QList<Task> IStorage::getTasks() {
	QMutexLocker lock(&mutex);
	return tasks;
}

int IStorage::totalTasks() {
	QMutexLocker lock(&mutex);
	return tasks.size();
}

// Searches all descriptions of all tasks in memory for specified keyword(s).
// Returns a list of all tasks that contain the keyword in its description.
// Searches by any part of the description. Case insensitive is the default.
QList<Task> IStorage::searchByDescription(QString keyword, Qt::CaseSensitivity caseSensitivity) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Searching by description keyword: " << keyword.toStdString();
	QList<Task> results;

	for (int i=0; i<tasks.size(); i++) {
		QStringRef description(&tasks[i].getDescription());
		if (description.contains(keyword, caseSensitivity)) {
			results.push_back(tasks[i]);
		}
	}

	return results;
}

// Searches all tags in all tasks in memory for specified tag.
// Returns a list of all tasks that contain that tag.
// Will also return partial results (if tag contains the searched keyword)
// Case sensitivity optional, but insensitive is the default.
QList<Task> IStorage::searchByTag(QString keyword, Qt::CaseSensitivity caseSensitivity) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Searching by tag: " << keyword.toStdString();
	QList<Task> results;

	for (int i=0; i<tasks.size(); i++) {
		QList<QString> tags = tasks[i].getTags();
		int tagCount = tags.size();

		for (int j=0; j<tagCount; j++) {
			if (tags[j].contains(keyword, caseSensitivity)) {
				results.push_back(tasks[i]);
			}
		}
	}

	return results;
}

// Searches all tasks for those that ends by (end date is no later than) 
// specified date-time.
QList<Task> IStorage::searchByEndDate(QDateTime byThisDate) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Searching by end date/time.";
	QList<Task> results;

	for (int i=0; i<tasks.size(); i++) {
		if (tasks[i].getEnd() <= byThisDate) {
			results.push_back(tasks[i]);
		}
	}

	return results;
}

// Searches all tasks for those that begins from (start date is no earlier than) 
// specified date-time.
QList<Task> IStorage::searchByBeginDate(QDateTime fromThisDate) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Searching by begin date/time.";
	QList<Task> results;

	for (int i=0; i<tasks.size(); i++) {
		if (tasks[i].getBegin() >= fromThisDate) {
			results.push_back(tasks[i]);
		}
	}

	return results;
}

// Searches all tasks that exist within a specified date-time interval.
// For a task to qualify, its start date must be no earlier than fromThisDate, and
// its end date must be no later than byThisDate.
QList<Task> IStorage::searchByDateTimeInterval(QDateTime fromThisDate, QDateTime byThisDate) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Searching by date/time interval.";
	QList<Task> results;

	for (int i=0; i<tasks.size(); i++) {
		bool fromStart = tasks[i].getBegin() >= fromThisDate;
		bool byEnd = tasks[i].getEnd() <= byThisDate;

		if (fromStart && byEnd) {
			results.push_back(tasks[i]);
		}
	}

	return results;
}

void IStorage::sortByEndDate() {
	LOG(INFO) << "Sorting by end date.";
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		if (!t1.getEnd().isValid()) {
			return true;
		}
		return t1.getEnd() < t2.getEnd();
	});
}

void IStorage::sortByBeginDate() {
	LOG(INFO) << "Sorting by begin date.";
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.getBegin() < t2.getBegin();
	});
}

void IStorage::sortByDescription() {
	LOG(INFO) << "Sorting by description.";
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.getDescription().toLower() < t2.getDescription().toLower();
	});
}

void IStorage::sortByOngoing() {
	LOG(INFO) << "Sorting by done status.";
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.isOngoing() > t2.isOngoing();
	});
}

void IStorage::sortByOverdue() {
	LOG(INFO) << "Sorting by done status.";
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.isOverdue() > t2.isOverdue();
	});
}

void IStorage::sortByDone() {
	LOG(INFO) << "Sorting by done status.";
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.isDone() < t2.isDone();
	});
}

void IStorage::sortByHasBeginDate() {
	LOG(INFO) << "Sorting by done status.";
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.getBegin().isValid() > t2.getBegin().isValid();
	});
}

// Renumbers the ID of all tasks in memory naively.
void IStorage::renumber() {

	sortByDescription();
	sortByEndDate();
	sortByHasBeginDate();
	sortByDone();
	sortByOngoing();
	sortByOverdue();
	
	for (int i=0; i<tasks.size(); i++) {
		tasks[i].setId(i);
	}
}

void IStorage::clearAllDone() {
	LOG(INFO) << "Clearing all tasks marked as done.";
	foreach (const Task& task, tasks) {
		if (task.isDone()) {
			tasks.removeOne(task);
		}
	}
}

void IStorage::clearAllTasks() {
	LOG(INFO) << "Clearing all tasks with great justice.";
	tasks.clear();
}

// Constructor for Storage.
Storage::Storage() {
	LOG(INFO) << "Storage instance created...";

	qRegisterMetaType<Task>("Task");
	qRegisterMetaTypeStreamOperators<Task>("Task");	
}

Storage::Storage(QString path) {
	LOG(INFO) << "Storage instance with custom save directory created...";

	this->path = path;

	qRegisterMetaType<Task>("Task");
	qRegisterMetaTypeStreamOperators<Task>("Task");	
}

// This function loads the contents of the text file and serialize it into
// the memory. If there is no such file, this function does nothing.
void Storage::loadFile() {
	LOG(INFO) << "Loading file...";

	renumber();

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");

	int size = settings.beginReadArray("Tasks");
	for (int i=0; i<size; i++) {
		settings.setArrayIndex(i);
		Task task;

		task.setDescription(settings.value("Description").toString());
		uint beginTime = settings.value("BeginTimeUnix", 0).toUInt();
		if (beginTime != 0) {
			task.setBegin(QDateTime::fromTime_t(settings.value("BeginTimeUnix").toInt()));
		}
		uint endTime = settings.value("EndTimeUnix", 0).toUInt();
		if (endTime != 0) {
			task.setEnd(QDateTime::fromTime_t(settings.value("EndTimeUnix").toInt()));
		}
		
		task.setDone(settings.value("Done").toBool());

		int tagCount = settings.beginReadArray("Tags");
		for (int j=0; j<tagCount; j++) {
			settings.setArrayIndex(j);
			QString tag = settings.value("Tag").toString();
			task.addTag(tag);
		}
		settings.endArray();

		tasks.push_back(task);
	}
	settings.endArray();

	LOG(INFO) << "File loaded.";
}

// This function deserialize the data from memory and writes it to the text
// file. If the file cannot be written, an ExceptionNotOpen is thrown.
void Storage::saveFile() {
	LOG(INFO) << "Saving file...";

	renumber();

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");

	settings.clear();
	settings.beginWriteArray("Tasks");
	for (int i=0; i<tasks.size(); i++) {
		settings.setArrayIndex(i);
		settings.setValue("Description", tasks[i].getDescription());
		settings.setValue("BeginTime", tasks[i].getBegin().toString());
		settings.setValue("EndTime", tasks[i].getEnd().toString());

		if (tasks[i].getBegin().isNull() || !tasks[i].getBegin().isValid()) {
			settings.setValue("BeginTimeUnix", "");
		} else {
			settings.setValue("BeginTimeUnix", tasks[i].getBegin().toTime_t());
		}

		if (tasks[i].getEnd().isNull() || !tasks[i].getEnd().isValid()) {
			settings.setValue("EndTimeUnix", "");
		} else {
			settings.setValue("EndTimeUnix", tasks[i].getEnd().toTime_t());
		}

		settings.setValue("Done", tasks[i].isDone());

		settings.beginWriteArray("Tags");
		QList<QString> tags = tasks[i].getTags();
		for (int j=0; j<tags.size(); j++) {
			settings.setArrayIndex(j);
			settings.setValue("Tag", tags[j]);

		}
		settings.endArray();
		settings.sync();
	}
	settings.endArray();
	settings.sync();

	LOG(INFO) << "File saved.";
}