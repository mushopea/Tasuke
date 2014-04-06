#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <glog/logging.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QMouseEvent>
#include <QListView>
#include <QKeySequence>
#include <QPoint>
#include <QPropertyAnimation>
#include <assert.h>
#include "Task.h"
#include "ui_TaskWindow.h"
#include "HotKeyThread.h"
#include "TaskEntry.h"
#include "TutorialWidget.h"

class TaskWindow : public QMainWindow {
	Q_OBJECT
	Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)

public:
	TaskWindow(QWidget *parent = 0);
	virtual ~TaskWindow();

	// Handles task list display
	void jumpToCurrentlySelectedTask();
	void updateCurrentlySelectedTo(int taskID);
	void highlightTask(int taskID);
	void highlightCurrentlySelectedTask(int prevsize);
	void showTasks(const QList<Task>& tasks, const QString& title = "");

	// Handles scrolling
	void scrollUp();
	void scrollDown();
	void pageUp();
	void pageDown();

	// Stacked widget functions
	int getScreen() const; // Helps decide which key press events to execute
	void changeTutorialWidgetTabs(); // Handles key press "tab"
	void showListWidget();
	void showTutorialWidget();

public slots:
	void showAndMoveToSide();
	void handleEmptyAddTaskButton();
	void handleBackButton();

protected:
	void closeEvent(QCloseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	bool eventFilter(QObject* object, QEvent* event);


private:

	//========================================
	// ATTRIBUTES
	//=========================================
	static const int TASKS_PER_PAGE = 5;


	enum class SubheadingType : char {
		OVERDUE,
		DUE_TODAY,
		TIMED,
		FLOATING,
		SUBHEADING_TYPE_LAST_ITEM
	};

	Ui::TaskWindowClass ui;	
	TutorialWidget tutorial;
	HotKeyThread *hotKeyThread;
	QPoint mpos;
	QList<Task> currentTasks;
	QPropertyAnimation *animation;
    QProgressBar *progressBar;
	qreal wOpacity;

	// For selection of tasks
	int currentlySelectedTask; // Represents of the # of the selected entry in the UI. (Range: 1 < # < taskListSize)
	int previouslySelectedTask;
	int previousSize;
	int subheadingRowIndexes[SubheadingType::SUBHEADING_TYPE_LAST_ITEM];

	//========================================
	// FUNCTIONS
	//=========================================
	void resetSubheadingIndexes();
	void initTutorial();
	void initAnimation();
	void decideContent();
	void showBackButtonIfSearching(const QString& title);
	void setOpacity(qreal value);
	qreal getOpacity() const;

	
	// Handles task entry creation and addition to list
	bool isInRange(int taskID) const;
	void changeTitle(const QString& title);
	TaskEntry* createEntry(const Task& t);
	void addListItemToRow(TaskEntry* entry, int row, const QString& type);
	void addListItem(TaskEntry* entry);
	void displaySubheading(const QString& content);
	void displayTask(const Task& t, int showDone);
	int getTaskEntryRow(int taskID) const;
};

#endif // TASKWINDOW_H
