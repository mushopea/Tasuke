#ifndef TASKENTRY_H
#define TASKENTRY_H

#include <QWidget>
#include <qdatetime.h>
#include "ui_TaskEntry.h"
#include <QFontMetrics>

class TaskEntry : public QWidget
{
	Q_OBJECT

public:
	TaskEntry(int id, QString description, QList<QString> tags, QDateTime start, QDateTime end, QWidget *parent = 0);
	~TaskEntry();
	
	Ui::TaskEntry ui;


public slots:

protected:

private slots:
	

private:
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
	void setDescription(QString description);
	void setDateTimes(QDateTime start, QDateTime end);
	QString createTagString(QList<QString> tags);
	void setTags(QList<QString> tags);
	void makeWidget();
};

#endif // TASKENTRY_H
