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
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private slots:
	void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
	Ui::TaskWindowClass ui;
	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;
	QPoint mpos;
	QAction *quitAction;
	QAction *showInputWindowAction;
	QAction *showTaskWindowAction;
	QAction *showSettingsWindowAction;
	QAction *showHelpWindowAction;
	QAction *showAboutWindowAction;

};

#endif // TASKWINDOW_H
