#ifndef TASK_H
#define TASK_H

#include <QList>
#include <QSet>
#include <QString>
#include <QDateTime>

//@author A0096863M
class Task {
private:
	QString description;
	QSet<QString> tags;

	QDateTime begin;
	QDateTime end;

	bool done;
	int id;

public:
	Task();
	Task(QString _description);
	~Task();

	void setDescription(QString _description);
	QString getDescription() const;

	void addTag(QString _tag);
	bool removeTag(QString _tag);
	QList<QString> getTags() const;
	QSet<QString> getTagsSet() const;

	void setBegin(QDateTime _begin);
	void setBeginDate(QDate _beginDate);
	void setBeginTime(QTime _beginTime);
	QDateTime getBegin() const;

	void setEnd(QDateTime _end);
	void setEndDate(QDate _endDate);
	void setEndTime(QTime _endTime);
	QDateTime getEnd() const;

	QString getTimeDifferenceString() const;

	void setDone(bool _done);
	void markDone();
	void markUndone();
	bool isDone() const;

	void setId(int _id);
	int getId() const;

	bool isFloating() const;
	bool isOverdue() const;
	bool isOngoing() const;
	bool isDueToday() const;
	bool isDueTomorrow() const;
	bool isDueOn(QDate _date) const;
	bool isEvent() const;

	bool operator==(const Task& other) const;
	bool operator!=(const Task& other) const;
	
	friend QDataStream& operator<<(QDataStream& out, const Task& task);
	friend QDataStream& operator>>(QDataStream& in, Task& task);
};

Q_DECLARE_METATYPE(Task)

#endif