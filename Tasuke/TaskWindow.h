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
	void showMessage(QString message);
	void contextMenuOperations();

public slots:
	void showAndMoveToSide();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
	Ui::TaskWindowClass ui;
	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;
	QAction *quitAction;
	QAction *showInputWindowAction;
	QAction *showTaskWindowAction;
	QAction *settingsAction;
	QAction *helpAction;
	QAction *showAboutWindowAction;

};

#endif // TASKWINDOW_H