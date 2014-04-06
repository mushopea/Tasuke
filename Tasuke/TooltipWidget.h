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
	void setText(InputStatus status, QString content = "");

public slots:
	void showAndAlign();
	void initIcons();

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
	void setIconOnLabel(InputStatus status);
	bool connectedToSettings; 
};

#endif //TOOLTIPWIDGET_H
