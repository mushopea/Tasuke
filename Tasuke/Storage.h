#ifndef STORAGE_H
#define STORAGE_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include "Task.h"

// Interface class for Storage.
class IStorage {
protected:
	//QList<Task> tasks;
	QList<QSharedPointer<Task>> tasks;
	QMutex mutex;

public:
	IStorage();
	virtual ~IStorage();

	Task addTask(Task& task);
	Task& getTask(int pos);
	void removeTask(int pos);
	void popTask();
	QList<Task> getTasks() const;
	int totalTasks();

	QList<Task> searchByDescription(QString keyword, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);
	QList<Task> searchByTag(QString keyword, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);
	QList<Task> searchByEndDate(QDateTime byThisDate);
	QList<Task> searchByBeginDate(QDateTime fromThisDate);
	QList<Task> searchByDateTimeInterval(QDateTime fromThisDate, QDateTime byThisDate);

	void sortByEndDate();
	void sortByBeginDate();
	void sortByDescription();
	void sortByOngoing();
	void sortByDone();
	void sortByOverdue();

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
	void loadFile();
	void saveFile();
};

#endif
