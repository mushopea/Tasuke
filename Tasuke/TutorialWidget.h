#ifndef TUTORIALWIDGET_H
#define TUTORIALWIDGET_H

#include <glog/logging.h>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include "ui_TutorialWidget.h"


class TutorialWidget : public QWidget {
	Q_OBJECT

public:
	TutorialWidget(QWidget *parent = 0);
	~TutorialWidget();

public slots:
	void backToTasuke();
	void changeTabs();

protected:
	bool eventFilter(QObject* object, QEvent* event);

private slots:


private:
	Ui::TutorialWidget ui;

};

#endif // TUTORIALWIDGET_H
