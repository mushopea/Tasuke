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
	QDateTime end;
	bool done;
	int id;

public:
	Task();
	~Task();

	void setDescription(QString _description);
	QString getDescription() const;

	void addTag(QString tag);
	void removeTag(QString tag);
	QList<QString> getTags();

	void setBegin(QDateTime _begin);
	QDateTime getBegin() const;

	void setEnd(QDateTime _end);
	QDateTime getEnd() const;

	void setDone(bool _done);
	void markDone();
	void markUndone();
	bool isDone() const;

	void setId(int identifier);
	int getId();

	bool isOverdue();
	bool isOngoing();
	
	bool operator!=(const Task& other) const;
	bool operator==(const Task& other) const;
	bool operator<(const Task& other) const;
	bool operator<=(const Task& other) const;
	bool operator>(const Task& other) const;
	bool operator>=(const Task& other) const;
	

	friend QDataStream& operator<<(QDataStream& out, const Task& task);
	friend QDataStream& operator>>(QDataStream& in, Task& task);
};

Q_DECLARE_METATYPE(Task)

#endif