#include <glog/logging.h>
#include <QApplication>
#include <QDesktopWidget>
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

void TutorialWidget::backToTasuke() {
	Tasuke::instance().getTaskWindow().showListWidget();
}