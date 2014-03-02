#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QtWidgets/QMainWindow>
#include "Task.h"
#include "ui_TaskWindow.h"

class TaskWindow : public QMainWindow {
	Q_OBJECT

public:
	TaskWindow(QWidget *parent = 0);
	~TaskWindow();

	void showTasks(QList<Task> tasks);

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
	Ui::TaskWindowClass ui;
	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;
	QAction *quitAction;
};

#endif // TASKWINDOW_H
