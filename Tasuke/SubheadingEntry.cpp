#include "Tasuke.h"
#include "SubheadingEntry.h"
#include <QSettings>

//@author A0100189

// Subheading entry is entered inside the listWidget inside Task Window.
// It is used to sort and organize the tasks visually for the user.
SubheadingEntry::SubheadingEntry(const QString& content, QWidget *parent): QWidget(parent), connectedToSettings(false) {
	initUI();
	initSettingsConnect();
	reloadTheme();
	changeText(content);
}

SubheadingEntry::~SubheadingEntry() {

}

void SubheadingEntry::changeText(const QString& text) {
	ui.content->setText(text);
}

void SubheadingEntry::reloadTheme() {
	LOG(INFO) << "Reloading theme in SubheadingEntry";
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	Theme currTheme = (Theme)settings.value("Theme", (char)Theme::DEFAULT).toInt();

	switch (currTheme) {
	case Theme::DEFAULT:
		applyDefaultTheme();
		break;
	case Theme::GREEN:
		applyGreenTheme();
		break;
	case Theme::SPACE:
		applySpaceTheme();
		break;
	case Theme::PINK:
		applyPinkTheme();
		break;
	case Theme::PIKACHU:
		applyPikaTheme();
		break;
	case Theme::BLUE:
		applyBlueTheme();
		break;
	case Theme::DOGE:
		applyDogeTheme();
		break;
	default:
		break;
	}
}

void SubheadingEntry::initUI() {
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
}

void SubheadingEntry::initSettingsConnect() {
	if (!connectedToSettings) {
		connectedToSettings = true;
		connect(&Tasuke::instance().getSettingsWindow(), SIGNAL(themeChanged()), this, SLOT(reloadTheme()));
		LOG(INFO) << "Connected SubheadingEntry to SettingsWindow";
	}
}

void SubheadingEntry::applyDefaultTheme() {
	setStyleSheet("QLabel{\n"
		"color: rgb(99, 99, 99);\n"
		"}\n"
		"background: transparent;");
}

void SubheadingEntry::applyGreenTheme() {
	setStyleSheet("QLabel{\n"
		"color: rgb(84,117,17);\n"
		"}\n"
		"background: transparent;");
}

void SubheadingEntry::applySpaceTheme() {
	setStyleSheet("QLabel{\n"
		"color: rgb(228,235,255);\n"
		"}\n"
		"background: transparent;");
}

void SubheadingEntry::applyPinkTheme() {
	setStyleSheet("QLabel{\n"
		"color: rgb(215,137,156);\n"
		"}\n"
		"background: transparent;");
}

void SubheadingEntry::applyPikaTheme() {
	setStyleSheet("QLabel{\n"
		"color: rgb(120,89,49);\n"
		"}\n"
		"background: transparent;");
}

void SubheadingEntry::applyBlueTheme() {
	setStyleSheet("QLabel{\n"
		"color: rgb(77,124,169);\n"
		"}\n"
		"background: transparent;");
}

void SubheadingEntry::applyDogeTheme() {
	setStyleSheet("QLabel{\n"
		"color: rgb(0,255,255);\n"
		"}\n"
		"background: transparent;"
		"font: 75 16pt \"Comic Sans MS\";");
}