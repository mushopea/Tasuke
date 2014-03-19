#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include <QWidget>
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

protected:
	bool eventFilter(QObject *object, QEvent *event);
	void closeEvent(QCloseEvent* event);

private slots:
	void handleReturnPressed();
	void handleEditingFinished();
	void handleHotKeyPress(int key);
	void changeBorder(int themeNumber);
	void changeBG(int themeNumber);

private:
	Ui::InputWindow ui;
	HotKeyThread *hotKeyThread;
    InputHighlighter *highlighter;
};

#endif // INPUTWINDOW_H
