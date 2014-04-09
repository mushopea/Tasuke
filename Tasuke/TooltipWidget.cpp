#include "Tasuke.h"
#include "TooltipWidget.h"
#include <QSettings>

//@author A0100189

TooltipWidget::TooltipWidget(QWidget *parent) : QWidget(parent), font("Consolas", 11), fm(font), 
												animation(this, "pos"), connectedToSettings(false) {
	LOG(INFO) << "TooltipWidget instance created";

	ui.setupUi(this);
	initIcons();
	initAnimation();

	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
}

TooltipWidget::~TooltipWidget() {
}

void TooltipWidget::setText(InputStatus status, QString content) {
	setIconOnLabel(status);

	// only update text if provided
	if (!content.isEmpty()) {
		ui.text->setText(content);
		fitWidthToTextLength(content);
	}
}

void TooltipWidget::showAndAlign() {
	LOG(INFO) << "Displaying tooltip widget";

	if (!connectedToSettings) {
		LOG(INFO) << "Connecting settingswindow to tooltipwidget";
		connectedToSettings = true;
		connect(&Tasuke::instance().getSettingsWindow(), SIGNAL(iconsChanged()), this, SLOT(initIcons()));
	}

	QPoint posBefore;
	posBefore.setY(Tasuke::instance().getInputWindow().y() + Tasuke::instance().getInputWindow().height() - 20);
	posBefore.setX(Tasuke::instance().getInputWindow().x() + 8);

	QPoint posAfter;
	posAfter.setY(Tasuke::instance().getInputWindow().y() + Tasuke::instance().getInputWindow().height());
	posAfter.setX(Tasuke::instance().getInputWindow().x() + 8);

	animation.setStartValue(posBefore);
	animation.setEndValue(posAfter);
	show();

	animation.start();
}

void TooltipWidget::initIcons() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	IconSet iconSet = (IconSet)settings.value("Icon", (char)IconSet::NYANSUKE).toInt();

	switch (iconSet) {
		case IconSet::NYANSUKE:
			normalIcon = QPixmap(":Images/images/icons/nyansukenormal.png");
			successIcon = QPixmap(":Images/images/icons/nyansukesuccess.png");
			failureIcon = QPixmap(":Images/images/icons/nyansukefailure.png");
			break;
		case IconSet::SYMBOLS:
			normalIcon = QPixmap(":Images/images/icons/symbolnormal.png");
			successIcon = QPixmap(":Images/images/icons/symbolsuccess.png");
			failureIcon = QPixmap(":Images/images/icons/symbolfailure.png");
			break;
		case IconSet::SYMBOLS2:
			normalIcon = QPixmap(":Images/images/icons/symbolnormal.png");
			successIcon = QPixmap(":Images/images/icons/symbolwhitesuccess.png");
			failureIcon = QPixmap(":Images/images/icons/symbolwhitefailure.png");
			break;
		case IconSet::NICCAGE:
			normalIcon = QPixmap(":Images/images/icons/niccagenormal.png");
			successIcon = QPixmap(":Images/images/icons/niccagesuccess.png");
			failureIcon = QPixmap(":Images/images/icons/niccagefailure.png");
			break;
		case IconSet::MEME:
			normalIcon = QPixmap(":Images/images/icons/memenormal.png");
			successIcon = QPixmap(":Images/images/icons/memesuccess.png");
			failureIcon = QPixmap(":Images/images/icons/memefailure.png");
			break;
		case IconSet::SHIBE:
			normalIcon = QPixmap(":Images/images/icons/dogenormal.png");
			successIcon = QPixmap(":Images/images/icons/dogesuccess.png");
			failureIcon = QPixmap(":Images/images/icons/dogefailure.png");
			break;
		default:
			break;
	}
}

void TooltipWidget::fitWidthToTextLength(QString text) {
	ui.bg->resize(fm.width(text) + 70, ui.bg->height()); 
}

void TooltipWidget::initAnimation() {
	animation.setDuration(500);
	animation.setEasingCurve(QEasingCurve::OutCubic);
}

void TooltipWidget::setIconOnLabel(InputStatus status) {
	switch (status) {
		case InputStatus::SUCCESS:
			ui.icon->setPixmap(successIcon);
			break;
		case InputStatus::FAILURE:
			ui.icon->setPixmap(failureIcon);
			break;
		case InputStatus::NORMAL:
			ui.icon->setPixmap(normalIcon);
			break;
		default:
			break;
	}
}
