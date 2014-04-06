#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include <QWidget>
#include <glog/logging.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QPropertyAnimation>
#include "HotKeyThread.h"
#include "ui_InputWindow.h"
#include "InputHighlighter.h"
#include "TooltipWidget.h"
#include "Constants.h"

class InputWindow : public QWidget {
	Q_OBJECT
	Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)

public:
	InputWindow(QWidget *parent = 0);
	~InputWindow();
	void showTooltipMessage(QString message, InputStatus status);
	void hideTooltip();

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
	void handleLineEditChanged();

private:
	Ui::InputWindow ui;
    InputHighlighter *highlighter;
	TooltipWidget *tooltipWidget;
	QPropertyAnimation *animation;
	qreal wOpacity;

	void setOpacity(qreal value);
	qreal getOpacity() const;
};

#endif // INPUTWINDOW_H
