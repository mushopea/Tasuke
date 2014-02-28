#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_TaskWindow.h"

class TaskWindow : public QMainWindow {
	Q_OBJECT

public:
	TaskWindow(QWidget *parent = 0);
	~TaskWindow();

private:
	Ui::TaskWindowClass ui;
};

#endif // TASKWINDOW_H
