//@author A0100189M

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
#include "InputHighlighter.h"
#include "TooltipWidget.h"
#include "Constants.h"
#include "ui_InputWindow.h"

// The input window is also the command box. 
// The input window handles text input, keyword highlighting and displaying of command tooltip guide.

class InputWindow : public QWidget {
	Q_OBJECT
	Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)

public:
	InputWindow(QWidget *parent = 0);
	~InputWindow();

	void showTooltipMessage(InputStatus status, QString message = "");
	void hideTooltip();
	void doErrorAnimation();
	void showAndCenter();	
	void showAndAdd();
	void closeAndClear();

public slots:
	void handleReloadTheme();
	void handleReloadFeatures();

protected:
	bool eventFilter(QObject *object, QEvent *event) override;
	void hideEvent(QHideEvent *event) override;

signals:
	void inputChanged(QString text);
	void reloadIcons();

private slots:
	void handleReturnPressed();
	void handleLineEditChanged();

private:
	
	// ====================================================
	//	Attributes
	// ====================================================
	Ui::InputWindow ui;
    InputHighlighter *highlighter;
	TooltipWidget *tooltipWidget;
	QPropertyAnimation animation;
	QPropertyAnimation errorAnimation;
	qreal wOpacity;
	bool showTooltip;
	
	// ====================================================
	//	Functions
	// ====================================================
	void setOpacity(qreal value);
	qreal getOpacity() const;
	
	// For initialization
	void initUI();
	void initWidgets();
	void initUIConnect();
	void initAnimation();
	void initErrorAnimation();
};

#endif // INPUTWINDOW_H
