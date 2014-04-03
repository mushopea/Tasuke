#ifndef TASKENTRY_H
#define TASKENTRY_H

#include <QWidget>
#include "ui_TaskEntry.h"
#include <QFontMetrics>
#include <QDateTime>
#include "Task.h"

class TaskEntry : public QWidget
{
	Q_OBJECT

public:
	TaskEntry(int id, Task t, QWidget *parent = 0);
	~TaskEntry();
	void strikeOut();
	void highlightOngoing();
	void highlightOverdue();
	Ui::TaskEntry ui;

public slots:

protected:

private slots:
	

private:
	static const int MAX_WIDTH_FOR_DESCRIPTION = 336;
	static const int MAX_WIDTH_FOR_TAGS = 160;

	// fields
	int id;
	Task task;

	// font metrics
	QFont font;
	QFontMetrics fm;

	// functions
	void setDescription(QString description);
	void setDateTimes(QDateTime start, QDateTime end);
	QString createTagString(QList<QString> tags);
	void setTags(QList<QString> tags);
	void makeWidget();
};

#endif // TASKENTRY_H
