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
	void reset();
	void goNextPage();
	void goPrevPage();

public slots:
	void goToFirstPage();
	void backToTasuke();
	void changeTabs();
	void handleNavNext();
	void handleNavPrev();
	void handleCommandsNext();
	void handleCommandsPrev();
	void handleFeaturesNext();
	void handleFeaturesPrev();

protected:
	bool eventFilter(QObject* object, QEvent* event);

private slots:


private:
	Ui::TutorialWidget ui;
	static const int NAV_PAGE = 0;
	static const int COMMANDS_PAGE = 1;
	static const int FEATURES_PAGE = 2;


	int getNextIndex(int currIndex, int max);
	int getPrevIndex(int currIndex, int max);
};

#endif // TUTORIALWIDGET_H
