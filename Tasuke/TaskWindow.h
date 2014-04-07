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
	void highlightTask(int taskID);
	void showTasks(const QList<Task>& tasks, const QString& title = "");

	// Handles scrolling (public because InputWindow accesses)
	void scrollUp();
	void scrollDown();
	void pageUp();
	void pageDown();
	void gotoPreviousSection();
	void gotoNextSection();

	// Stacked widget functions
	int getScreen() const; // Helps decide which key press events to execute
	void changeTutorialWidgetTabs(); // Handles key press "tab"
	void showListWidget();
	void showTutorialWidget();

public slots:
	void showAndMoveToSide();
	void handleEmptyAddTaskButton();
	void handleBackButton();
	void displayTaskList();

protected:
	void closeEvent(QCloseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	bool eventFilter(QObject* object, QEvent* event);


private:

	//=========================================
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
	QPoint mpos;
	qreal wOpacity;
	bool connectedToSettings;
	QList<Task> currentTasks;
	QPropertyAnimation animation;
    QProgressBar progressBar;
	TutorialWidget tutorial;
	HotKeyThread *hotKeyThread;

	// For selection of tasks
	bool onlyShowDone;
	int currentlySelectedTask;
	int previouslySelectedTask;
	int previousSize;
	int subheadingRowIndexes[SubheadingType::SUBHEADING_TYPE_LAST_ITEM];

	//=========================================
	// HELPER FUNCTIONS
	//=========================================

	// For initialization
	void initTutorial();
	void initAnimation();
	void initProgressBar();
	void setOpacity(qreal value);
	qreal getOpacity() const;

	//  Private helper functions for task display
	TaskEntry* createEntry(const Task& t);
	void addListItemToRow(TaskEntry* entry, int row, const QString& type);
	void addListItem(TaskEntry* entry);
	void displayTask(const Task& t);
	int getTaskEntryRow(int taskID) const;

	// Private helper functions for subheadings display
	void displayAndUpdateSubheadings(int index);	
	void resetSubheadingIndexes();
	void displaySubheading(const QString& content);

	// Private helper functions for window content display
	void decideContent();
	void showBackButtonIfSearching(const QString& title);
	void changeTitle(const QString& title);
	void hideProgressBarWhenDone();

	// Private helper functions for scrolling and highlighting of tasks
	bool isInRange(int taskID) const;
	void updateCurrentlySelectedTo(int taskID);	
	void jumpToCurrentlySelectedTask();
	void highlightCurrentlySelectedTask(int prevsize);
};

#endif // TASKWINDOW_H
