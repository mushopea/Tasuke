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

public slots:
	void showAndCenter();

protected:
	void closeEvent(QCloseEvent* event);

private slots:
	void handleReturnPressed();
	void handleEditingFinished();
	void changeBorder(int themeNumber);
	void changeBG(int themeNumber);

private:
	Ui::InputWindow ui;
};

#endif // INPUTWINDOW_H
