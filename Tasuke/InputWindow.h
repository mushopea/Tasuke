#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include <QWidget>
#include "ui_InputWindow.h"

class InputWindow : public QWidget
{
	Q_OBJECT

public:
	InputWindow(QWidget *parent = 0);
	~InputWindow();

private:
	Ui::InputWindow ui;
};

#endif // INPUTWINDOW_H
