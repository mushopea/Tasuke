#include "Tasuke.h"
#include "SubheadingEntry.h"
#include "Exceptions.h"
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
	disconnect(this, SLOT(reloadTheme()));
}

void SubheadingEntry::changeText(const QString& text) {
	ui.content->setText(text);
}

// Essentially reloads the theme's font color
void SubheadingEntry::reloadTheme() {
	LOG(INFO) << "Reloading theme in SubheadingEntry";

	// Get current theme ID
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	Theme currTheme = (Theme)settings.value("Theme", (char)Theme::DEFAULT).toInt();
	try {
		if (currTheme < (Theme)0 && currTheme >= Theme::THEME_LAST_ITEM) {
			throw ExceptionThemeOutOfRange();
		} else {
			setStyleSheet(ThemeStylesheets::SUBHEADING_STYLES[(char)currTheme]); // Apply theme
		}
	} catch (ExceptionThemeOutOfRange *exception) {
		// If the icon enum in the settings is out of range, set back to default
		settings.setValue("Theme", (char)Theme::DEFAULT); 
		reloadTheme();
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
		LOG(INFO) << "Connected SubheadingEntry to SettingsWindow";
		connect(parentWidget(), SIGNAL(themeChanged()), this, SLOT(reloadTheme()));
	}
}