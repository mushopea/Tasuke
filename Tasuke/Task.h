#ifndef TASK_H
#define TASK_H

#include <QList>
#include <QString>
#include <QDateTime>

class Task {
private:
	QString description;
	QList<QString> tags;

	QDateTime begin;
	QDate beginDate;
	QTime beginTime;

	QDateTime end;
	QDate endDate;
	QTime endTime;

	bool done;
	int id;

public:
	Task();
	Task(QString description);
	~Task();

	void setDescription(QString _description);
	QString getDescription() const;

	void addTag(QString tag);
	void removeTag(QString tag);
	QList<QString> getTags();

	void setBegin(QDateTime _begin);
	void setBeginDate(QDate _beginDate);
	void setBeginTime(QTime _beginTime);
	QDateTime getBegin() const;

	void setEnd(QDateTime _end);
	void setEndDate(QDate _endDate);
	void setEndTime(QTime _endTime);
	QDateTime getEnd() const;

	QString getTimeLeftString() const;

	void setDone(bool _done);
	void markDone();
	void markUndone();
	bool isDone() const;

	void setId(int identifier);
	int getId();

	bool isFloating() const;
	bool isOverdue() const;
	bool isOngoing() const;
	bool isDueToday() const;
	bool isDueTomorrow() const;
	bool isEvent() const;
	
	bool operator!=(const Task& other) const;
	bool operator==(const Task& other) const;/*
	bool operator<(const Task& other) const;
	bool operator<=(const Task& other) const;
	bool operator>(const Task& other) const;
	bool operator>=(const Task& other) const;*/
	
	friend QDataStream& operator<<(QDataStream& out, const Task& task);
	friend QDataStream& operator>>(QDataStream& in, Task& task);
};

Q_DECLARE_METATYPE(Task)

#endif