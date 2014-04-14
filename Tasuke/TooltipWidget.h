//@author A0100189M

#ifndef TOOLTIPWIDGET_H
#define TOOLTIPWIDGET_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <QFontMetrics>
#include <QFont>
#include <glog/logging.h>
#include "Constants.h"
#include "ui_TooltipWidget.h"

// The tooltip guide displays when user is inputting a command. It aims to guide the user on formats.
// It can be called by logic when it wants to send feedback.
// The signals of the user inputting command is handled by the input window.
// Hence, tooltip widget's parent is the input window.

class TooltipWidget : public QWidget {
	Q_OBJECT

public:
	TooltipWidget(QWidget *parent = 0);
	~TooltipWidget();

	void setText(InputStatus status, QString content = "");
	void showAndAlign();

public slots:
	void handleReloadIcons();

private:
	static const int WIDTH_DIFFERENCE = 70;
	Ui::TooltipWidget ui;
	QPropertyAnimation animation;
	QFont font;
	QFontMetrics fm;
	QPixmap normalIcon;
	QPixmap successIcon;
	QPixmap failureIcon;

	void fitWidthToTextLength(QString text);
	void setIconOnLabel(InputStatus status);
	void initUI();
	void initAnimation();
	void resetAnimation();
};

#endif //TOOLTIPWIDGET_H
