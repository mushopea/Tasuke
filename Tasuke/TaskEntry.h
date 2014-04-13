#ifndef TASKENTRY_H
#define TASKENTRY_H

#include <QWidget>
#include <QFontMetrics>
#include <QDateTime>
#include <QToolTip>
#include "ui_TaskEntry.h"
#include "Task.h"

//@author A0100189m

// Each TaskEntry object represents a Task with different ID, description, dates and tags.
// They are entered into the listWidget in TaskWindow.

class TaskEntry : public QWidget {
	Q_OBJECT

public:
	TaskEntry(const Task& t, QWidget *parent = 0);
	~TaskEntry();

	Ui::TaskEntry ui;
	void highlightOngoing();
	void highlightOverdue();

private:
	// Attributes
	static const int FONT_SIZE_DIFF = 4;
	enum class TaskEntryLabel : char {
		ID,
		DESCRIPTION,
		START_DATE,
		START_TIME,
		END_DATE,
		END_TIME,
		TAG,
		TASKENTRYLABEL_LAST_ITEM
	};

	const Task& task;
	QLabel* labels[(char)TaskEntryLabel::TASKENTRYLABEL_LAST_ITEM];

	// Functions
	QString createTagString(const QList<QString>& tags) const;
	void setID(int ID);
	void setTooltip(
		const QString& description,
		const QDateTime& start, 
		const QDateTime& end, 
		const QList<QString>& tags, 
		const QString& dueInMessage);
	void setDescription(const QString& description);
	void setDateTimes(const QDateTime& start, const QDateTime& end);
	void setTags(const QList<QString>& tags);

	// For initialization
	void initLabelsArray();
	void initFonts();
	void initUI();
	void makeWidget();
};

#endif // TASKENTRY_H
