#include "Tasuke.h"
#include "TooltipWidget.h"
#include <QSettings>

//@author A0100189

// The tooltip guide displays when user is inputting a command. It aims to guide the user on formats.
// It can be called by logic when it wants to send feedback.
// The signals of the user inputting command is handled by the input window.
// Hence, tooltip widget's parent is the input window.

TooltipWidget::TooltipWidget(QWidget *parent) : QWidget(parent), font("Consolas", 11), fm(font), 
												animation(this, "pos") {
	LOG(INFO) << "TooltipWidget instance created";
	initUI();
	initIcons();
	initAnimation();
}

TooltipWidget::~TooltipWidget() {
}

// Changes the text shown on the window then resizes it.
void TooltipWidget::setText(InputStatus status, QString content) {
	setIconOnLabel(status);

	// Only update text if provided
	if (!content.isEmpty()) {
		ui.text->setText(content);
		fitWidthToTextLength(content);
	}
}

// Shows the widget with animation
void TooltipWidget::showAndAlign() {
	LOG(INFO) << "Displaying tooltip widget";
	resetAnimation();
	show();
	animation.start();
}

// Initialize icon set from settings
void TooltipWidget::initIcons() {
	LOG(INFO) << "Initialising tooltip widget icons";
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

// Modifies width according to text width
void TooltipWidget::fitWidthToTextLength(QString text) {
	QTextDocument document;
	document.setDefaultFont(QFont("Consolas", 11));
	document.setHtml(text);
	ui.bg->resize(document.idealWidth() + WIDTH_DIFFERENCE, ui.bg->height()); 
}

// Sets the icon according to the status of the input
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

// ===================================
// initialization
// ===================================

void TooltipWidget::initUI() {
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
}

// ===================================
// animation
// ===================================

void TooltipWidget::initAnimation() {
	animation.setDuration(500);
	animation.setEasingCurve(QEasingCurve::OutCubic);
}

void TooltipWidget::resetAnimation() {
	QPoint posBefore;
	posBefore.setY(Tasuke::instance().getInputWindow().y() + Tasuke::instance().getInputWindow().height() - 20);
	posBefore.setX(Tasuke::instance().getInputWindow().x() + 8);

	QPoint posAfter;
	posAfter.setY(Tasuke::instance().getInputWindow().y() + Tasuke::instance().getInputWindow().height());
	posAfter.setX(Tasuke::instance().getInputWindow().x() + 8);

	animation.setStartValue(posBefore);
	animation.setEndValue(posAfter);
}