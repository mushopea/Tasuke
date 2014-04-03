#include "Tasuke.h"
#include "TooltipWidget.h"

TooltipWidget::TooltipWidget(QWidget *parent) : QWidget(parent) {

	ui.setupUi(this);

	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);

}

TooltipWidget::~TooltipWidget() {

}


void TooltipWidget::showAndAlign() {


}