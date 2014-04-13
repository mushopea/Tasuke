#include <QSettings>
#include "Tasuke.h"
#include "Exceptions.h"
#include "SettingsWindow.h"

//@author A0100189m

SettingsWindow::SettingsWindow(QWidget* parent) : QWidget(parent) {
	LOG(INFO) << "SettingsWindow instance created";

	initUI();
	initUIConnect();
	initIconsArray();
	initThemeArray();
}

SettingsWindow::~SettingsWindow() {
	LOG(INFO) << "SettingsWindow instance destroyed";
}

// Shows and centers the Settings Window, then resets the tab to the first one.
void SettingsWindow::showAndCenter() {
	LOG(INFO) << "Displaying settings window";

	QPoint pos = QApplication::desktop()->screen()->rect().center() - rect().center();
	pos.setY(QApplication::desktop()->screen()->rect().height() / 4);

	move(pos);
	show(); 
	raise(); 
	ui.tabWidget->setCurrentIndex(0);
}

// Switches tabs to the next tab
void SettingsWindow::changeTabs() {

	int nextTab;
	int currTab = ui.tabWidget->currentIndex();

	if (currTab == ui.tabWidget->count()-1) {
		nextTab = 0;
	} else { 
		nextTab = currTab + 1;
	}

	ui.tabWidget->setCurrentIndex(nextTab);
}


// ============================================================
// slots
// ============================================================

// Handles the user pressing Apply - all the options they changed will be set.
void SettingsWindow::handleApplyButton() {
	editFeatures();
	editFont();
	editIcons();
	editRunOnStartup();
	editTheme();
}

// Like apply, but also closes the Settings Window.
void SettingsWindow::handleOKButton() {
	handleApplyButton();
	close();
}

// ============================================================
// Event handler
// ============================================================

bool SettingsWindow::eventFilter(QObject* object, QEvent* event) {
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);
		if (eventKey->key() == Qt::Key_Tab) {
			LOG(INFO) << "Settings Key_Tab pressed";
			changeTabs();
			return true;
		}
	}
	return QObject::eventFilter(object, event);
}

void SettingsWindow::showEvent(QShowEvent *event) {
	QWidget::showEvent(event);
	loadCurrSettings();
}

// ============================================================
// Initializers
// ============================================================

void SettingsWindow::initUI() {
	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint | Qt::Tool);
}

void SettingsWindow::initUIConnect() {
	connect(ui.OK, SIGNAL(pressed()), this, SLOT(handleOKButton()));
	connect(ui.apply, SIGNAL(pressed()), this, SLOT(handleApplyButton()));
}

void SettingsWindow::initIconsArray() {
	iconSelectButtons[(char)IconSet::MEME] = ui.optionMeme;
	iconSelectButtons[(char)IconSet::NICCAGE] = ui.optionNicCage;
	iconSelectButtons[(char)IconSet::NYANSUKE] = ui.optionNyansuke;
	iconSelectButtons[(char)IconSet::SHIBE] = ui.optionShibe;
	iconSelectButtons[(char)IconSet::SYMBOLS] = ui.optionSymbols;
	iconSelectButtons[(char)IconSet::SYMBOLS2] = ui.optionSymbols2;
}

void SettingsWindow::initThemeArray() {
	themeSelectButtons[(char)Theme::DEFAULT] = ui.selectDefault;
	themeSelectButtons[(char)Theme::GREEN] = ui.selectGreen;
	themeSelectButtons[(char)Theme::SPACE] = ui.selectSpace;
	themeSelectButtons[(char)Theme::PINK] = ui.selectPink;
	themeSelectButtons[(char)Theme::PIKACHU] = ui.selectPika;
	themeSelectButtons[(char)Theme::BLUE] = ui.selectBlue;
	themeSelectButtons[(char)Theme::DOGE] = ui.selectDefault;
}

// ============================================================
// Helper functions to load current settings
// =============================================================

// This function will initialize the options according to current settings.
void SettingsWindow::loadCurrSettings() {
	LOG(INFO) << "Loading current settings into settings window.";

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	loadCurrFeatures();
	loadCurrFont();
	loadCurrIcons();
	loadCurrTheme();
}

// Load up feature selections
void SettingsWindow::loadCurrFeatures() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");

	bool currRunOnStartup = settings.value("RunOnStartup", true).toBool();
	ui.runOnStartup->setChecked(currRunOnStartup);

	bool currEnableCommandHighlight = settings.value("HighlightCommandsEnabled", true).toBool();
	ui.commandColors->setChecked(currEnableCommandHighlight);

	bool currEnableSpellcheck = settings.value("SpellcheckEnabled", true).toBool();
	ui.spellcheck->setChecked(currEnableSpellcheck);

	bool currEnableTooltip = settings.value("TooltipEnabled", true).toBool();
	ui.commandTooltip->setChecked(currEnableTooltip);
}

// Load up current font selection
void SettingsWindow::loadCurrFont() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	QString fontSelected = settings.value("Font", "Print Clearly").toString();
	ui.fontSelect->setCurrentFont(QFont(fontSelected, ui.fontSelect->currentFont().pointSize()));
}

// Load up current iconset selection
void SettingsWindow::loadCurrIcons() {

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	IconSet currIconSet = (IconSet)settings.value("Icon", (char)IconSet::NYANSUKE).toInt();

	try {
		if (currIconSet < (IconSet)0 && currIconSet >= IconSet::ICONSET_LAST_ITEM) {
			throw ExceptionIconsetOutOfRange();
		} else {
			iconSelectButtons[(char)currIconSet]->setChecked(true); // check the option if valid icon enum
		}
	} catch (ExceptionIconsetOutOfRange *exception) {
		// If the icon enum in the settings is out of range, set back to default
		settings.setValue("Icon", (char)IconSet::NYANSUKE); 
		loadCurrIcons();
	}
}

// Load up current theme selection
void SettingsWindow::loadCurrTheme() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	Theme currTheme = (Theme)settings.value("Theme", (char)Theme::DEFAULT).toInt();
	try {
		if (currTheme < (Theme)0 && currTheme >= Theme::THEME_LAST_ITEM) {
			throw ExceptionThemeOutOfRange();
		} else {
			themeSelectButtons[(char)currTheme]->setChecked(true); // check the option if valid icon enum
		}
	} catch (ExceptionThemeOutOfRange *exception) {
		// If the icon enum in the settings is out of range, set back to default
		settings.setValue("Theme", (char)Theme::DEFAULT); 
		loadCurrTheme();
	}
}

// ============================================================
// Handles settings change
// ============================================================

// Change run on startup
void SettingsWindow::editRunOnStartup() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	bool oldRunOnStartup = settings.value("RunOnStartup", true).toBool();
	bool newRunOnStartup = ui.runOnStartup->isChecked();
}

// Change features in settings
void SettingsWindow::editFeatures() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	settings.setValue("HighlightCommandsEnabled", ui.commandColors->isChecked());	
	settings.setValue("SpellcheckEnabled", ui.spellcheck->isChecked());	
	settings.setValue("TooltipEnabled", ui.commandTooltip->isChecked());
	emit featuresChanged();
}

// Change font in settings
void SettingsWindow::editFont() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	QString oldFont = settings.value("Font", "Print Clearly").toString();
	QString fontSelected = ui.fontSelect->currentFont().family();
	if (fontSelected.compare(oldFont) != 0) {
		settings.setValue("Font", fontSelected);
		LOG(INFO) << "Font changed to " << fontSelected.toStdString() << " from " << oldFont.toStdString();
		emit fontChanged();
	}
}

// Change Icon Set in settings
void SettingsWindow::editIcons() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	IconSet oldIcons = (IconSet)settings.value("Icon", (char)IconSet::NYANSUKE).toInt();
	for (int i = 0; i < (char)IconSet::ICONSET_LAST_ITEM; ++i) {
		if (iconSelectButtons[i]->isChecked()) {
			settings.setValue("Icon", i);
			if (oldIcons != (IconSet)i) {
				LOG(INFO) << "Tooltip feedback icon has changed.";
				emit iconsChanged();
			}
			break;
		}
	}
}

// Change theme in settings. It will emit the signal to TaskWindow, InputWindow and SubheadingEntry to change theme.
void SettingsWindow::editTheme() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	Theme oldTheme = (Theme)settings.value("Theme", (char)Theme::DEFAULT).toInt();
	for (int i=0; i< (char)Theme::THEME_LAST_ITEM - 1; ++i) {
		if (themeSelectButtons[i]->isChecked()) {
			settings.setValue("Theme", i);
			if (oldTheme != (Theme)i) {
				LOG(INFO) << "Theme has been changed.";
				emit themeChanged();
			}
			break;
		}
	}
}