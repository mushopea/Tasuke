#ifndef TASKENTRY_H
#define TASKENTRY_H

#include <QWidget>
#include "ui_TaskEntry.h"
#include <QFontMetrics>
#include <QDateTime>
#include "Task.h"

class TaskEntry : public QWidget {
	Q_OBJECT

public:
	TaskEntry(const Task& t, QWidget *parent = 0);
	~TaskEntry();
	void highlightOngoing();
	void highlightOverdue();
	Ui::TaskEntry ui;

public slots:

protected:

private slots:
	

private:
	static const int FONT_SIZE_DIFF = 4;

	enum class TaskEntryLabel : char
	{
		ID,
		DESCRIPTION,
		START_DATE,
		START_TIME,
		END_DATE,
		END_TIME,
		TAG,
		TASKENTRYLABEL_LAST_ITEM
	};

	// fields
	const Task& task;
	QLabel* labels[(char)TaskEntryLabel::TASKENTRYLABEL_LAST_ITEM];

	// functions
	void initLabelsArray();
	void initFonts();
	void setTooltip(const QString& des, const QDateTime& start, const QDateTime& end, 
					const QList<QString>& tags, const QString& dueInMessage);
	void setDescription(const QString& description);
	void setDateTimes(const QDateTime& start, const QDateTime& end);
	QString createTagString(const QList<QString>& tags) const;
	void setTags(const QList<QString>& tags);
	void makeWidget();
};

#endif // TASKENTRY_H
