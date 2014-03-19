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

	
	void showTasks(QList<Task> tasks);
	
public slots:
	void showAndMoveToSide();

protected:
	void closeEvent(QCloseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyDownEvent(QKeyEvent *event);

	void handleHotKeyPress(int key);
private:
	Ui::TaskWindowClass ui;	
	HotKeyThread *hotKeyThread;
	QPoint mpos;

};

#endif // TASKWINDOW_H
