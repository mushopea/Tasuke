#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QtWidgets/QMainWindow>
#include <QMouseEvent>
#include <QListView>
#include <QBitmap>
#include <QKeySequence>
#include <QPoint>
#include "Task.h"
#include "ui_TaskWindow.h"
#include "HotKeyThread.h"
#include "TaskEntry.h"
#include "TutorialWidget.h"

class TaskWindow : public QMainWindow {
	Q_OBJECT

public:
	TaskWindow(QWidget *parent = 0);
	virtual ~TaskWindow();

	void highlightCurrentlySelected(int prevsize);
	void focusOnNewTask();
	void showTasks(QList<Task> tasks);
	void initTut();
	void scrollUp();
	void scrollDown();
	void pageUp();
	void pageDown();
	int getScreen();

	
public slots:
	void showListWidget();
	void showTutorialWidget();
	void showAndMoveToSide();

protected:
	void closeEvent(QCloseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	bool eventFilter(QObject* object, QEvent* event);


private:
	static const int TASKS_PER_PAGE = 5;
	static const int TASK_ENTRY_WIDTH = 780;
	static const int TASK_ENTRY_HEIGHT = 65;

	Ui::TaskWindowClass ui;	
	TutorialWidget *tut;
	HotKeyThread *hotKeyThread;
	QPoint mpos;
	QList<Task> currentTasks;
	int currentlySelected; //represents of the # of the selected entry in the UI. min is 1. max is size of the task list
	int previouslySelected;
	int previousSize;

};

#endif // TASKWINDOW_H
