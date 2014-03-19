#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QtWidgets/QMainWindow>
#include <QMouseEvent> 
#include <QPoint>
#include "Task.h"
#include "ui_TaskWindow.h"
#include "HotKeyThread.h"
#include <qscrollbar.h>

class TaskWindow : public QMainWindow {
	Q_OBJECT

public:
	TaskWindow(QWidget *parent = 0);
	~TaskWindow();

	void highlightCurrentlySelected();
	void showTasks(QList<Task> tasks);
	void scrollUp();
	void scrollDown();
	void pageUp();
	void pageDown();

	
public slots:
	void showAndMoveToSide();

protected:
	void closeEvent(QCloseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	bool eventFilter(QObject* object, QEvent* event);

	void handleHotKeyPress(int key);

private:
	Ui::TaskWindowClass ui;	
	HotKeyThread *hotKeyThread;
	QPoint mpos;
	QList<Task> currentTasks;
	int currentlySelected;
	int previouslySelected;

};

#endif // TASKWINDOW_H
