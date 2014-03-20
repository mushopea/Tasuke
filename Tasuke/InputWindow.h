#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include <QWidget>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QBitmap>
#include <QPropertyAnimation>
#include "HotKeyThread.h"
#include "ui_InputWindow.h"
#include "InputHighlighter.h"

class InputWindow : public QWidget {
	Q_OBJECT

public:
	InputWindow(QWidget *parent = 0);
	~InputWindow();

public slots:
	void showAndCenter();

	bool eventFilter(QObject *object, QEvent *event);
private slots:
	void handleReturnPressed();
	void handleEditingFinished();
	void changeBorder(int themeNumber);
	void changeBG(int themeNumber);

private:
	Ui::InputWindow ui;
    InputHighlighter *highlighter;
};

#endif // INPUTWINDOW_H
