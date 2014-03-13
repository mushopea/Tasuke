#ifndef TASKENTRY_H
#define TASKENTRY_H

#include <QWidget>
#include <qdatetime.h>
#include "ui_TaskEntry.h"
#include <qfontmetrics.h>

class TaskEntry : public QWidget
{
	Q_OBJECT

public:
	TaskEntry(int id, QString description, QList<QString> tags, QDateTime start, QDateTime end, QWidget *parent = 0);
	~TaskEntry();

public slots:

protected:

private slots:


private:
	Ui::TaskEntry ui;
	static const int MAX_WIDTH_FOR_DESCRIPTION = 336;
	static const int MAX_WIDTH_FOR_TAGS = 160;

	//fields
	int id;
	QString description;
	QList<QString> tags;
	QDateTime start;
	QDateTime end;

	//font metrics
	QFont font;
	QFontMetrics fm;

	//functions
	QString fitDescription(QString description, int currWidth);
	QString fitTags(QString tag, int currWidth);
	void setDescription(QString description);
	void setDateTimes(QDateTime start, QDateTime end);
	void setTags(QList<QString> tags);
	void makeWidget();
};

#endif // TASKENTRY_H
