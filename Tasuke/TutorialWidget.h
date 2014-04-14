//@author A0100189M

#ifndef TUTORIALWIDGET_H
#define TUTORIALWIDGET_H

#include <glog/logging.h>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include "ui_TutorialWidget.h"

// Tutorial widget handles the tutorial slideshow. 
// Hence, it has many Next and Prev buttons to go through pages.

class TutorialWidget : public QWidget {
	Q_OBJECT

public:
	TutorialWidget(QWidget *parent = 0);
	~TutorialWidget();

	void reset();
	void goNextPage();
	void goPrevPage();
	void goToFirstPage();
	void changeTabs();

public slots:
	void handleBackButton();
	void handleNavNext();
	void handleNavPrev();
	void handleCommandsNext();
	void handleCommandsPrev();
	void handleFeaturesNext();
	void handleFeaturesPrev();

protected:
	bool eventFilter(QObject* object, QEvent* event) override;

private:
	Ui::TutorialWidget ui;

	// page constants
	static const int NAV_PAGE = 0;
	static const int COMMANDS_PAGE = 1;
	static const int FEATURES_PAGE = 2;

	// initializations
	void initUI();
	void initConnect();
	
	// index getter
	int getNextIndex(int currIndex, int max);
	int getPrevIndex(int currIndex, int max);
};

#endif // TUTORIALWIDGET_H
