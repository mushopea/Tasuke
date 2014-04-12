#include "Tasuke.h"
#include "TutorialWidget.h"

//@author A0100189

// Tutorial widget handles the tutorial slideshow. Hence, it has many Next and Prev buttons.
TutorialWidget::TutorialWidget(QWidget* parent) : QWidget(parent) {
	LOG(INFO) << "Tutorialwidget instance created";
	initUI();
	initConnect();
}

TutorialWidget::~TutorialWidget() {
	LOG(INFO) << "TutorialWidget instance destroyed";
}

// Everything is back to first page
void TutorialWidget::reset() {
	ui.tabWidget->setCurrentIndex(0);
	ui.navSlideshow->setCurrentIndex(0);
	ui.commandsSlideshow->setCurrentIndex(0);
	ui.featuresSlideshow->setCurrentIndex(0);
}

// Goes to the next page of the current tutorial
void TutorialWidget::goNextPage() {
	int tabNumber = ui.tabWidget->currentIndex();
	if (tabNumber == NAV_PAGE) {
		handleNavNext();
	} else if (tabNumber == COMMANDS_PAGE) {
		handleCommandsNext();
	} else if (tabNumber == FEATURES_PAGE) {
		handleFeaturesNext();
	}
}

// Goes to the previous page of the current tutorial
void TutorialWidget::goPrevPage() {
	int tabNumber = ui.tabWidget->currentIndex();
	if (tabNumber == NAV_PAGE) {
		handleNavPrev();
	} else if (tabNumber == COMMANDS_PAGE) {
		handleCommandsPrev();
	} else if (tabNumber == FEATURES_PAGE) {
		handleFeaturesPrev();
	}
}

void TutorialWidget::goToFirstPage() {
	ui.tabWidget->setCurrentIndex(0);
}
// Goes back to the task list
void TutorialWidget::backToTasuke() {
	Tasuke::instance().getTaskWindow().showListWidget();
}

// Switches tabs to the next tab
void TutorialWidget::changeTabs() {

	int nextTab;
	int currTab = ui.tabWidget->currentIndex();

	if (currTab == ui.tabWidget->count()-1) {
		nextTab = 0;
	} else { 
		nextTab = currTab + 1;
	}

	ui.tabWidget->setCurrentIndex(nextTab);
}

void TutorialWidget::handleNavNext() {
	ui.navSlideshow->slideInIdx(getNextIndex(ui.navSlideshow->currentIndex(), ui.navSlideshow->count()));
}

void TutorialWidget::handleNavPrev() {
	ui.navSlideshow->slideInIdx(getPrevIndex(ui.navSlideshow->currentIndex(), ui.navSlideshow->count()));
}

void TutorialWidget::handleCommandsNext() {
	ui.commandsSlideshow->slideInIdx(getNextIndex(ui.commandsSlideshow->currentIndex(), ui.commandsSlideshow->count()));
}

void TutorialWidget::handleCommandsPrev() {
	ui.commandsSlideshow->slideInIdx(getPrevIndex(ui.commandsSlideshow->currentIndex(), ui.commandsSlideshow->count()));
}

void TutorialWidget::handleFeaturesNext() {
	ui.featuresSlideshow->slideInIdx(getNextIndex(ui.featuresSlideshow->currentIndex(), ui.featuresSlideshow->count()));
}

void TutorialWidget::handleFeaturesPrev() {
	ui.featuresSlideshow->slideInIdx(getPrevIndex(ui.featuresSlideshow->currentIndex(), ui.featuresSlideshow->count()));
}

bool TutorialWidget::eventFilter(QObject* object, QEvent* event) {
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);
		if (eventKey->key() == Qt::Key_Tab) {
			changeTabs();
			return true;
		}
	}
	return QObject::eventFilter(object, event);
}

// ================================================
//	INITIALISATION
// ================================================

void TutorialWidget::initUI() {
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
}

void TutorialWidget::initConnect() {
	connect(ui.pushButton, SIGNAL(pressed()), this, SLOT(backToTasuke()));
	connect(ui.navPrev, SIGNAL(pressed()), this, SLOT(handleNavPrev()));
	connect(ui.navNext, SIGNAL(pressed()), this, SLOT(handleNavNext()));
	connect(ui.featuresPrev, SIGNAL(pressed()), this, SLOT(handleFeaturesPrev()));
	connect(ui.featuresNext, SIGNAL(pressed()), this, SLOT(handleFeaturesNext()));
	connect(ui.commandsPrev, SIGNAL(pressed()), this, SLOT(handleCommandsPrev()));
	connect(ui.commandsNext, SIGNAL(pressed()), this, SLOT(handleCommandsNext())); 
}


// ================================================
//	PAGE INDEX GETTERS
// ================================================

// Returns the next page number of the stacked widget
int TutorialWidget::getNextIndex(int currIndex, int max) {
	if (currIndex == max-1) {
		return 0;
	} else {
		return currIndex + 1;
	}
}

// Returns the previous page number of the stacked widget
int TutorialWidget::getPrevIndex(int currIndex, int max) {
	if (currIndex == 0) {
		return max - 1;
	} else {
		return currIndex - 1;
	}
}