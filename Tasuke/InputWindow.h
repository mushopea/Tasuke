#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include <QWidget>
#include <glog/logging.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QBitmap>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include "HotKeyThread.h"
#include "ui_InputWindow.h"
#include "InputHighlighter.h"

class InputWindow : public QWidget {
	Q_OBJECT
	Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)


public:
	InputWindow(QWidget *parent = 0);
	~InputWindow();

public slots:
	void showAndCenter();	
	void showAndAdd();

protected:
	bool eventFilter(QObject *object, QEvent *event);

private slots:
	void handleReturnPressed();
	void handleEditingFinished();
	void initAnimation();
	void changeBorder(int themeNumber);
	void changeBG(int themeNumber);

private:
	Ui::InputWindow ui;
    InputHighlighter *highlighter;
	QPropertyAnimation *animation;
	qreal wOpacity;

	void setOpacity(qreal value);
	qreal getOpacity();

};

#endif // INPUTWINDOW_H
