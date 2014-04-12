#include "Tasuke.h"
#include "SubheadingEntry.h"
#include "ThemeStylesheets.h"
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
	char currTheme = (char)settings.value("Theme", (char)Theme::DEFAULT).toInt();
	setStyleSheet(ThemeStylesheets::SUBHEADING_STYLES[currTheme]);
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