//@author A0100189M

#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <glog/logging.h>
#include <assert.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QMenu>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QListView>
#include <QKeySequence>
#include <QPoint>
#include <QPropertyAnimation>
#include "Task.h"
#include "HotKeyThread.h"
#include "TaskEntry.h"
#include "TutorialWidget.h"
#include "ui_TaskWindow.h"

// The task window is the main window.
// It contains a stacked widget of two pages: The task list, and the tutorial widget.
// It handles the scrolling and focusing of tasks as well.

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

	// Window display functions
	void positionAndShow();
	void showTutorialOrTaskList();
	void showAndMoveToSide();


public slots:
	void handleAddTaskButton();
	void handleBackButton();
	void handleReloadTheme();
	void displayTaskList();

signals:
	void themeChanged();

protected:
	void closeEvent(QCloseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	bool eventFilter(QObject* object, QEvent* event) override;


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
	QList<Task> currentTasks;
	QPropertyAnimation animation;
    QProgressBar progressBar;
	TutorialWidget tutorial;
	HotKeyThread *hotKeyThread;

	// For selection of tasks
	int currentlySelectedTask;
	int previouslySelectedTask;
	int previousSize;
	int subheadingRowIndexes[SubheadingType::SUBHEADING_TYPE_LAST_ITEM];

	// For theming
	QString taskEntrySelectStylesheet;
	QString taskEntryNormalStylesheet;

	//=========================================
	// HELPER FUNCTIONS
	//=========================================

	// For initialization
	void initUI();
	void initUIConnect();
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
	void hideContent();
	void decideContent(QString title);
	void showBackButtonIfSearching(const QString& title);
	void changeTitle(const QString& title);
	void hideProgressBarWhenDone();

	// Private helper functions for scrolling and highlighting of tasks
	bool isInRange(int taskID) const;
	void updateCurrentlySelectedTo(int taskID);	
	void jumpToCurrentlySelectedTask();
	void highlightCurrentlySelectedTask(int prevsize);

	// Theming
	void applyTheme(const QString mainStyle, const QString normalTaskEntryStyle, const QString selectedTaskEntryStyle);
};

#endif // TASKWINDOW_H
