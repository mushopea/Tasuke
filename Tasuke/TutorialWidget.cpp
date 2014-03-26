#include "Tasuke.h"
#include "TutorialWidget.h"

TutorialWidget::TutorialWidget(QWidget* parent) : QWidget(parent) {
	LOG(INFO) << "Tutorialwidget instance created";

	ui.setupUi(this);

	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);

	connect(ui.pushButton, SIGNAL(pressed()), this, SLOT(backToTasuke()));
}

TutorialWidget::~TutorialWidget() {
	LOG(INFO) << "TutorialWidget instance destroyed";
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

bool TutorialWidget::eventFilter(QObject* object, QEvent* event) {
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);

		if (eventKey->key() == Qt::Key_Tab){
			changeTabs();
			return true;
		}
	}
	return QObject::eventFilter(object, event);

}