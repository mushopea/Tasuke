#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include <QWidget>
#include "HotKeyThread.h"
#include "ui_InputWindow.h"

class InputWindow : public QWidget
{
	Q_OBJECT

public:
	InputWindow(QWidget *parent = 0);
	~InputWindow();

	void showAndCenter();

protected:
	void closeEvent(QCloseEvent* event);

private slots:
	void handleReturnPressed();
	void handleEditingFinished();
	void handleHotKeyPress(int key);

private:
	Ui::InputWindow ui;
	HotKeyThread *hotKeyThread;
};

#endif // INPUTWINDOW_H
