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

//@author A0100189

class InputWindow : public QWidget {
	Q_OBJECT
	Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)

public:
	InputWindow(QWidget *parent = 0);
	~InputWindow();

	void showTooltipMessage(InputStatus status, QString message = "");
	void hideTooltip();
	void doErrorAnimation();

public slots:
	void showAndCenter();	
	void showAndAdd();
	void closeAndClear();
	void reloadTheme();

protected:
	bool eventFilter(QObject *object, QEvent *event);
	void hideEvent(QHideEvent *event);

signals:
	void inputChanged(QString text);

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
	bool connectedToSettings;
	
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
	void initSettingsConnect();

	// Functions that apply themes
	void applyDefaultTheme();
	void applyGreenTheme();
	void applySpaceTheme();
	void applyPinkTheme();
	void applyPikaTheme();
	void applyBlueTheme();
	void applyDogeTheme();
};

#endif // INPUTWINDOW_H
