//@author A0096863M
#define NOMINMAX

#include <glog/logging.h>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include "Constants.h"
#include "Exceptions.h"
#include "Storage.h"
#include "Tasuke.h"

IStorage::IStorage() {

}

IStorage::~IStorage() {

}

// Adds a task to the list of tasks in memory.
Task IStorage::addTask(Task& task) {
	QMutexLocker lock(&mutex);

	QSharedPointer<Task> taskPtr = QSharedPointer<Task>(new Task(task));

	LOG(INFO) << MSG_STORAGE_ADDING_TASK << task.getDescription().toStdString();

	tasks.push_back(taskPtr);
	renumber();

	return *taskPtr;
}

// Edits a task in memory.
// In general, this is done by replacing the task with ID id with
// a new task object.
Task IStorage::editTask(int id, Task& task) {
	QMutexLocker lock(&mutex);

	QSharedPointer<Task> taskPtr = QSharedPointer<Task>(new Task(task));

	LOG(INFO) << MSG_STORAGE_REPLACING_TASK << task.getDescription().toStdString();

	tasks.replace(id, taskPtr);
	renumber();

	return *taskPtr;
}

// Retrieves a task with ID id from the list of tasks in memory.
Task IStorage::getTask(int id) {
	QMutexLocker lock(&mutex);
	return *tasks[id];
}

// Removes a task with ID id from the list of tasks in memory.
void IStorage::removeTask(int id) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << MSG_STORAGE_REMOVING_TASK << id;

	tasks.removeAt(id);
	renumber();
}

// Removes a task from the back of the list of tasks in memory.
void IStorage::popTask() {
	QMutexLocker lock(&mutex);
	LOG(INFO) << MSG_STORAGE_POP_TASK;

	tasks.pop_back();
	renumber();
}

// Returns the task that is at the front of the list of tasks in
// memorry. This task is guaranteed not to be 'overdue'.
// This method throws ExceptionNoMoreTasks if there are no more tasks 
// in memory that are not overdue.
Task IStorage::getNextUpcomingTask() {
	LOG(INFO) << MSG_STORAGE_RETRIEVE_NEXT_TASK;

	foreach (QSharedPointer<Task> task, tasks) {
		if (task->getBegin() > QDateTime::currentDateTime()) {
			return *task;
		}
	}
	throw ExceptionNoMoreTasks();
}

// Read-only. Retrieves the entire list of tasks in memory.
QList<Task> IStorage::getTasks(bool hideDone) const {
	QList<Task> results;

	if (hideDone) {
		results = search([](Task task) -> bool {
			return !task.isDone();
		});
	} else {
		foreach (QSharedPointer<Task> task, tasks) {
			results.push_back(*task);
		}
	}

	return results;
}

// Returns the total number of tasks in memory.
int IStorage::totalTasks() {
	QMutexLocker lock(&mutex);
	return tasks.size();
}

// Searches for tasks. Takes in a function as an argument and searches for
// tasks with a criteria that is determined by the function. It is the caller's
// responsibility for the function to be valid and correct, as this method 
// makes no assumptions about the criteria.
QList<Task> IStorage::search(std::function<bool(Task)> predicate) const {
	LOG(INFO) << MSG_STORAGE_SEARCH;
	QList<Task> results;

	foreach(QSharedPointer<Task> task, tasks) {
		if (predicate(*task)) {
			results.push_back(*task);
		}
	}

	return results;
}

// Searches all descriptions of all tasks in memory for specified keyword(s).
// Returns a list of all tasks that contain the keyword in its description.
// Searches by any part of the description. Case insensitive is the default.
QList<Task> IStorage::searchByDescription(QString keyword, Qt::CaseSensitivity caseSensitivity) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << MSG_STORAGE_SEARCH_BY_DESCRIPTION << keyword.toStdString();
	QList<Task> results;

	for (int i=0; i<tasks.size(); i++) {
		if (tasks[i]->getDescription().contains(keyword, caseSensitivity)) {
			results.push_back(*tasks[i]);
		}
	}

	return results;
}

// Searches all tags in all tasks in memory for specified tag.
// Returns a list of all tasks that contain that tag.
// Will also return partial results (if tag contains the searched keyword)
// Case insensitive is the default.
QList<Task> IStorage::searchByTag(QString keyword, Qt::CaseSensitivity caseSensitivity) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << MSG_STORAGE_SEARCH_BY_TAG << keyword.toStdString();
	QList<Task> results;

	for (int i=0; i<tasks.size(); i++) {
		QList<QString> tags = tasks[i]->getTags();
		int tagCount = tags.size();

		for (int j=0; j<tagCount; j++) {
			if (tags[j].contains(keyword, caseSensitivity)) {
				results.push_back(*tasks[i]);
			}
		}
	}

	return results;
}

// Retrieves the next available free time.
// Starts by assuming that the current time is free.
// Then search through all tasks for ongoing events and take the ongoing event with the latest 
// end time as the next available free time.
// Then since tasks are sorted in order of earliest end time to latest end time,
// overlapping tasks will be merged into one unit.
QDateTime IStorage::nextFreeTime() {
	LOG(INFO) << MSG_STORAGE_NEXT_FREE_TIME;
	QDateTime nextAvailableTime = QDateTime::currentDateTime();

	foreach (const QSharedPointer<Task>& task, tasks) {
		if (task->getBegin() <= nextAvailableTime) {
			nextAvailableTime = task->getEnd();
		} else {
			break;
		}
	}
	return nextAvailableTime;
}

// Returns true if every task in memory is done.
// Returns false if any task in memory is not done.
bool IStorage::isAllDone() {
	bool _isAllDone = true;
	foreach (const QSharedPointer<Task>& task, tasks) {
		if (!task->isDone()) {
			_isAllDone = false;
		}
	}
	return _isAllDone;
}

// Sorts the list of tasks in memory by its end date-time.
// Tasks that end earlier are sorted to the front of the list.
void IStorage::sortByEndDate() {
	LOG(INFO) << MSG_STORAGE_SORT_BY_END_DATE;
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		return t1->getEnd() < t2->getEnd();
	});
}

// Sorts the list of tasks in memory by its description alphabetically.
void IStorage::sortByDescription() {
	LOG(INFO) << MSG_STORAGE_SORT_BY_DESCRIPTION;
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		return t1->getDescription().toLower() < t2->getDescription().toLower();
	});
}

// Sorts the list of tasks in memory by whether or not it is ongoing.
// Ongoing tasks are sorted to the front of the list.
void IStorage::sortByOngoing() {
	LOG(INFO) << MSG_STORAGE_SORT_BY_ONGOING_STATUS;
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		return t1->isOngoing() > t2->isOngoing();
	});
}

// Sorts the list of tasks in memory by whether or not it is due on
// the current day. Tasks that are due on the current day are sorted the front.
void IStorage::sortByIsDueToday() {
	LOG(INFO) << MSG_STORAGE_SORT_BY_DUE_TODAY;
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		return t1->isDueToday() > t2->isDueToday();
	});
}

// Sorts the list of tasks in memory by whether or not it is overdue.
// Tasks that are overdue are sorted to the front of the list.
void IStorage::sortByOverdue() {
	LOG(INFO) << MSG_STORAGE_SORT_BY_OVERDUE;
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		return t1->isOverdue() > t2->isOverdue();
	});
}

// Sorts the list of tasks in memory by whether or not it is done.
// Tasks that are done are sorted to the back of the list.
void IStorage::sortByDone() {
	LOG(INFO) << MSG_STORAGE_SORT_BY_DONE_STATUS;
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		return t1->isDone() < t2->isDone();
	});
}

// Sorts the list of tasks in memory by whether or not is has a valid
// end date-time. Tasks with no valid end date-time are sorted to the back
// of the list.
void IStorage::sortByHasEndDate() {
	LOG(INFO) << MSG_STORAGE_SORT_BY_HAS_END_DATE;
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		if (t1->getEnd().isValid() == t2->getEnd().isValid()) {
			return false;
		}

		if (t1->getEnd().isValid()) {
			return true;
		}

		return false;
	});
}

// Renumbers the ID of all tasks in memory naively.
void IStorage::renumber() {
	// Internally within groups sort by date then alphabetically
	sortByDescription();
	sortByEndDate();

	// Today is always below overdue
	sortByIsDueToday();

	// Overdue is always at the top
	sortByOverdue();

	// No end date is always above done
	sortByHasEndDate();

	// Done is alwayas at the bottom
	sortByDone();


	for (int i=0; i<tasks.size(); i++) {
		tasks[i]->setId(i);
	}
}

// Removes all tasks that are done from memory.
void IStorage::clearAllDone() {
	LOG(INFO) << MSG_STORAGE_CLEAR_ALL_DONE_TASKS;
	foreach (const QSharedPointer<Task>& task, tasks) {
		if (task->isDone()) {
			tasks.removeOne(task);
		}
	}
	renumber();
}

// Removes all tasks from memory regardless of status.
void IStorage::clearAllTasks() {
	LOG(INFO) << MSG_STORAGE_CLEAR_ALL_TASKS;
	tasks.clear();
	renumber();
}

// The default constructor for Storage automatically sets the path of the
// .ini save file to be %APPDATA%/Tasuke.
Storage::Storage() {
	LOG(INFO) << MSG_STORAGE_INSTANCE_CREATED;

	QDir dir = QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

	path = dir.absoluteFilePath("tasks.ini");

	qRegisterMetaType<Task>("Task");
	qRegisterMetaTypeStreamOperators<Task>("Task");
}

// This constructor for Storage takes in a filepath as an argument.
// Storage will read from and write to the .ini file at that path.
Storage::Storage(QString _path) {
	LOG(INFO) << MSG_STORAGE_INSTANCE_CREATED_NONDEFAULT;

	path = _path;

	qRegisterMetaType<Task>("Task");
	qRegisterMetaTypeStreamOperators<Task>("Task");
}

// This function loads the contents of the text file and serializes it into
// the memory. It does so via QSettings.
// If there is no such file, this function does nothing.
void Storage::loadFile() {
	LOG(INFO) << MSG_STORAGE_LOAD_FILE_START;

	QSettings settings(path, QSettings::IniFormat);

	int size = settings.beginReadArray("Tasks");
	for (int i=0; i<size; i++) {
		settings.setArrayIndex(i);
		Task* task = new Task();

		task->setDescription(settings.value("Description").toString());
		uint beginTime = settings.value("BeginTimeUnix", 0).toUInt();
		if (beginTime != 0) {
			task->setBegin(QDateTime::fromTime_t(settings.value("BeginTimeUnix").toInt()));
		}
		uint endTime = settings.value("EndTimeUnix", 0).toUInt();
		if (endTime != 0) {
			task->setEnd(QDateTime::fromTime_t(settings.value("EndTimeUnix").toInt()));
		}

		task->setDone(settings.value("Done").toBool());

		int tagCount = settings.beginReadArray("Tags");
		for (int j=0; j<tagCount; j++) {
			settings.setArrayIndex(j);
			QString tag = settings.value("Tag").toString();
			task->addTag(tag);
		}
		settings.endArray();

		tasks.push_back(QSharedPointer<Task>(task));
	}
	settings.endArray();

	renumber();
	NotificationManager::instance().init(this);

	LOG(INFO) << MSG_STORAGE_LOAD_FILE_END;
}

// This function deserializes the data from memory and writes it to the text
// file. It does so via QSettings.
// If the file cannot be written, an ExceptionNotOpen is thrown.
void Storage::saveFile() {
	LOG(INFO) << MSG_STORAGE_SAVE_FILE_START;

	QSettings settings(path, QSettings::IniFormat);

	settings.clear();
	settings.beginWriteArray("Tasks");
	for (int i=0; i<tasks.size(); i++) {
		settings.setArrayIndex(i);
		settings.setValue("Description", tasks[i]->getDescription());
		settings.setValue("BeginTime", tasks[i]->getBegin().toString());
		settings.setValue("EndTime", tasks[i]->getEnd().toString());

		if (tasks[i]->getBegin().isNull() || !tasks[i]->getBegin().isValid()) {
			settings.setValue("BeginTimeUnix", "");
		} else {
			settings.setValue("BeginTimeUnix", tasks[i]->getBegin().toTime_t());
		}

		if (tasks[i]->getEnd().isNull() || !tasks[i]->getEnd().isValid()) {
			settings.setValue("EndTimeUnix", "");
		} else {
			settings.setValue("EndTimeUnix", tasks[i]->getEnd().toTime_t());
		}

		settings.setValue("Done", tasks[i]->isDone());

		settings.beginWriteArray("Tags");
		QList<QString> tags = tasks[i]->getTags();
		for (int j=0; j<tags.size(); j++) {
			settings.setArrayIndex(j);
			settings.setValue("Tag", tags[j]);
		}
		settings.endArray();
		settings.sync();
	}
	settings.endArray();
	settings.sync();

	NotificationManager::instance().init(this);

	LOG(INFO) << MSG_STORAGE_SAVE_FILE_END;
}