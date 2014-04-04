#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <glog/logging.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QtWidgets/QMainWindow>
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
	void updateCurrentlySelectedTo(int row);
	void highlightTask(int row);
	void highlightCurrentlySelectedTask(int prevsize);
	void showTasks(QList<Task> tasks, QString title = "");

	// Handles scrolling
	void scrollUp();
	void scrollDown();
	void pageUp();
	void pageDown();

	// Stacked widget functions
	int getScreen(); // Helps decide which key press events to execute
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
	static const int TASK_ENTRY_WIDTH = 780;
	static const int TASK_ENTRY_HEIGHT = 65;

	Ui::TaskWindowClass ui;	
	TutorialWidget tutorial;
	HotKeyThread *hotKeyThread;
	QPoint mpos;
	QList<Task> currentTasks;
	QPropertyAnimation *animation;
	qreal wOpacity;

	// For selection of tasks
	int currentlySelected; // Represents of the # of the selected entry in the UI. (Range: 1 < # < taskListSize)
	int previouslySelected;
	int previousSize;

	//========================================
	// FUNCTIONS
	//=========================================
	void initTutorial();
	void initAnimation();
	void decideContent();
	void showBackButtonIfSearching(QString title);
	void setOpacity(qreal value);
	qreal getOpacity() const;

	
	// Handles task entry creation and addition to list
	bool isInRange(int index);
	void changeTitle(QString title);
	TaskEntry* createEntry(Task t, int index);
	void addListItemToRow(TaskEntry* entry, int row, QString type);
	void addListItem(TaskEntry* entry);
	void displayTask(Task t, int index, int showDone);
};

#endif // TASKWINDOW_H
