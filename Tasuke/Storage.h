#ifndef STORAGE_H
#define STORAGE_H

#include <functional>
#include <QString>
#include <QTimer>
#include <QList>
#include "Task.h"
#include "NotificationManager.h"

//@author A0096863M
// Interface class for Storage.
class IStorage {
protected:
	QList< QSharedPointer<Task> > tasks;
	QMutex mutex;

public:
	IStorage();
	virtual ~IStorage();

	Task addTask(Task& task);
	Task editTask(int id, Task& task);
	Task getTask(int id);
	void removeTask(int id);
	void popTask();
	Task getNextUpcomingTask();
	QList<Task> getTasks(bool hideDone = true) const;
	int totalTasks();

	QList<Task> search(std::function<bool(Task)> predicate) const;
	QList<Task> searchByDescription(QString keyword, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);
	QList<Task> searchByTag(QString keyword, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);

	QDateTime nextFreeTime();

	bool isAllDone();

	void sortByEndDate();
	void sortByBeginDate();
	void sortByDescription();
	void sortByOngoing();
	void sortByIsDueToday();
	void sortByDone();
	void sortByOverdue();
	void sortByHasEndDate();

	void renumber();

	void clearAllDone();
	void clearAllTasks();

	virtual void loadFile() = 0;
	virtual void saveFile() = 0;
};

// This class abstracts away the data management in memory and on disk.
// Usually only 1 instance of this class is required and it is managed by the
// Tasuke singleton.
class Storage : public IStorage {
private:
	QString path;
public:
	Storage();
	Storage(QString path);
	void loadFile() override;
	void saveFile() override;
};

#endif
