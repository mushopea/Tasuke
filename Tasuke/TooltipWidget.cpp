#include "Tasuke.h"
#include "TooltipWidget.h"

TooltipWidget::TooltipWidget(QWidget *parent) : QWidget(parent), font("Consolas", 11), fm(font) {
	LOG(INFO) << "TooltipWidget instance created";

	ui.setupUi(this);
	animation = new QPropertyAnimation(this, "pos");	
	animation->setDuration(500);
	animation->setEasingCurve(QEasingCurve::OutCubic);

	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
}

TooltipWidget::~TooltipWidget() {
}

void TooltipWidget::setText(QString content, InputStatus status) {
	ui.text->setText(content);

	fitWidthToTextLength(content);
}

void TooltipWidget::showAndAlign() {
	LOG(INFO) << "Displaying tooltip widget";

	QPoint posBefore;
	posBefore.setY(Tasuke::instance().getInputWindow().y() + Tasuke::instance().getInputWindow().height() - 20);
	posBefore.setX(Tasuke::instance().getInputWindow().x() + 8);

	QPoint posAfter;
	posAfter.setY(Tasuke::instance().getInputWindow().y() + Tasuke::instance().getInputWindow().height());
	posAfter.setX(Tasuke::instance().getInputWindow().x() + 8);

	animation->setStartValue(posBefore);
    animation->setEndValue(posAfter);
	show();

	animation->start();
}

void TooltipWidget::fitWidthToTextLength(QString text) {
	ui.bg->resize(fm.width(text) + 70, ui.bg->height()); 
}
