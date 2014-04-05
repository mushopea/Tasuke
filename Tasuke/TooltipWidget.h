#ifndef TOOLTIPWIDGET_H
#define TOOLTIPWIDGET_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <glog/logging.h>
#include "ui_TooltipWidget.h"
#include "Constants.h"
#include <QFontMetrics>
#include <QFont>


class TooltipWidget : public QWidget {
	Q_OBJECT

public:
	TooltipWidget(QWidget *parent = 0);
	~TooltipWidget();
	void setText(QString content, InputStatus status);

public slots:
	void showAndAlign();

private:
	Ui::TooltipWidget ui;
	QPropertyAnimation *animation;
	QFont font;
	QFontMetrics fm;
	QPixmap normalIcon;
	QPixmap successIcon;
	QPixmap failureIcon;

	void fitWidthToTextLength(QString text);
	void initAnimation();
	void initIcons();
};

#endif //TOOLTIPWIDGET_H
