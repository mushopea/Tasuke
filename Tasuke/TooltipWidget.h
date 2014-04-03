#ifndef TOOLTIPWIDGET_H
#define TOOLTIPWIDGET_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include "ui_TooltipWidget.h"


class TooltipWidget : public QWidget {
	Q_OBJECT

public:
	TooltipWidget(QWidget *parent = 0);
	~TooltipWidget();

public slots:
	void showAndAlign();

private:
	Ui::TooltipWidget ui;
};

#endif //TOOLTIPWIDGET_H
