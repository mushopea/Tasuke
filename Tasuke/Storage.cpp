#define NOMINMAX

#include <glog/logging.h>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <iostream>
#include "Constants.h"
#include "Exceptions.h"
#include "Storage.h"
#include "Tasuke.h"

//@author A0096863M
IStorage::IStorage() {

}

IStorage::~IStorage() {

}

Task IStorage::addTask(Task& task) {
	QMutexLocker lock(&mutex);

	QSharedPointer<Task> taskPtr = QSharedPointer<Task>(new Task(task));

	LOG(INFO) << "Adding task " << task.getDescription().toStdString();

	tasks.push_back(taskPtr);
	renumber();

	return *taskPtr;
}

Task IStorage::editTask(int id, Task& task) {
	QMutexLocker lock(&mutex);

	QSharedPointer<Task> taskPtr = QSharedPointer<Task>(new Task(task));

	LOG(INFO) << "Replacing task " << task.getDescription().toStdString();

	tasks.replace(id, taskPtr);
	renumber();

	return *taskPtr;
}

Task IStorage::getTask(int id) {
	QMutexLocker lock(&mutex);
	return *tasks[id];
}

void IStorage::removeTask(int id) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Removing task at position " << id;

	tasks.removeAt(id);
	renumber();
}

void IStorage::popTask() {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Popping task from back";

	tasks.pop_back();
	renumber();
}

Task IStorage::getNextUpcomingTask() {
	foreach (QSharedPointer<Task> task, tasks) {
		if (task->getBegin() > QDateTime::currentDateTime()) {
			return *task;
		}
	}
	throw ExceptionNoMoreTasks();
}

// Read-only
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

int IStorage::totalTasks() {
	QMutexLocker lock(&mutex);
	return tasks.size();
}

QList<Task> IStorage::search(std::function<bool(Task)> predicate) const {
	LOG(INFO) << "Searching for tasks";
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
	LOG(INFO) << "Searching by description keyword: " << keyword.toStdString();
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
// Case sensitivity optional, but insensitive is the default.
QList<Task> IStorage::searchByTag(QString keyword, Qt::CaseSensitivity caseSensitivity) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Searching by tag: " << keyword.toStdString();
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

// Searches all tasks for those that ends by (end date is no later than)
// specified date-time.
QList<Task> IStorage::searchByEndDate(QDateTime byThisDate) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Searching by end date/time.";
	QList<Task> results;

	for (int i=0; i<tasks.size(); i++) {
		if (tasks[i]->getEnd() <= byThisDate) {
			results.push_back(*tasks[i]);
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
		if (tasks[i]->getBegin() >= fromThisDate) {
			results.push_back(*tasks[i]);
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
		bool fromStart = tasks[i]->getBegin() >= fromThisDate;
		bool byEnd = tasks[i]->getEnd() <= byThisDate;

		if (fromStart && byEnd) {
			results.push_back(*tasks[i]);
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
	QDateTime nextAvailableTime = QDateTime::currentDateTime();

	foreach (const QSharedPointer<Task>& task, tasks) {

		// Skip if we have reached the part of the list where tasks begin on the next day.
		QDateTime breakCondition = QDateTime(QDate::currentDate().addDays(1), QTime(0, 0, 0));
		if (task->getBegin() >= breakCondition) {
			break;
		}

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

void IStorage::sortByEndDate() {
	LOG(INFO) << "Sorting by end date.";
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		return t1->getEnd() < t2->getEnd();
	});
}

void IStorage::sortByBeginDate() {
	LOG(INFO) << "Sorting by begin date.";
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		return t1->getBegin() < t2->getBegin();
	});
}

void IStorage::sortByDescription() {
	LOG(INFO) << "Sorting by description.";
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		return t1->getDescription().toLower() < t2->getDescription().toLower();
	});
}

void IStorage::sortByOngoing() {
	LOG(INFO) << "Sorting by ongoing status.";
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		return t1->isOngoing() > t2->isOngoing();
	});
}

void IStorage::sortByIsDueToday() {
	LOG(INFO) << "Sorting by due today.";
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		return t1->isDueToday() > t2->isDueToday();
	});
}

void IStorage::sortByOverdue() {
	LOG(INFO) << "Sorting by overdue status.";
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		return t1->isOverdue() > t2->isOverdue();
	});
}

void IStorage::sortByDone() {
	LOG(INFO) << "Sorting by done status.";
	qStableSort(tasks.begin(), tasks.end(), [](const QSharedPointer<Task>& t1, const QSharedPointer<Task>& t2) {
		return t1->isDone() < t2->isDone();
	});
}

void IStorage::sortByHasEndDate() {
	LOG(INFO) << "Sorting by presence of end date.";
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
	LOG(INFO) << "Clearing all tasks marked as done.";
	foreach (const QSharedPointer<Task>& task, tasks) {
		if (task->isDone()) {
			tasks.removeOne(task);
		}
	}
	renumber();
}

// Removes all tasks from memory regardless of status.
void IStorage::clearAllTasks() {
	LOG(INFO) << "Clearing all tasks with great justice.";
	tasks.clear();
	renumber();
}

// The default constructor for Storage automatically sets the path of the
// .ini save file to be %APPDATA%/Tasuke.
Storage::Storage() {
	LOG(INFO) << "Storage instance created...";

	QDir dir = QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

	path = dir.absoluteFilePath("tasks.ini");

	qRegisterMetaType<Task>("Task");
	qRegisterMetaTypeStreamOperators<Task>("Task");
}

// This constructor for Storage takes in a filepath as an argument.
// Storage will read from and write to the .ini file at that path.
Storage::Storage(QString _path) {
	LOG(INFO) << "Storage instance with custom path created...";

	path = _path;

	qRegisterMetaType<Task>("Task");
	qRegisterMetaTypeStreamOperators<Task>("Task");
}

// This function loads the contents of the text file and serializes it into
// the memory. If there is no such file, this function does nothing.
void Storage::loadFile() {
	LOG(INFO) << "Loading file...";

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
	NotificationManager::instance().init();

	LOG(INFO) << "File loaded.";
}

// This function deserializes the data from memory and writes it to the text
// file. If the file cannot be written, an ExceptionNotOpen is thrown.
void Storage::saveFile() {
	LOG(INFO) << "Saving file...";

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

	NotificationManager::instance().init();

	LOG(INFO) << "File saved.";
}
