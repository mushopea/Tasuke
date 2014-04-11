#include "Tasuke.h"
#include "SettingsWindow.h"
#include <QSettings>

//@author A0100189

SettingsWindow::SettingsWindow(QWidget* parent) : QWidget(parent) {
	LOG(INFO) << "SettingsWindow instance created";

	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint | Qt::Tool);

	connect(ui.OK, SIGNAL(pressed()), this, SLOT(handleOKButton()));
	connect(ui.apply, SIGNAL(pressed()), this, SLOT(handleApplyButton()));

	initIconsArray();
}

SettingsWindow::~SettingsWindow() {
	LOG(INFO) << "SettingsWindow instance destroyed";
}

void SettingsWindow::showEvent(QShowEvent *event) {
	QWidget::showEvent(event);
	loadCurrSettings();
}

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


void SettingsWindow::handleApplyButton() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");

	// font change
	QString oldFont = settings.value("Font", "Print Clearly").toString();
	QString fontSelected = ui.fontSelect->currentFont().family();
	if (fontSelected.compare(oldFont) != 0) {
		settings.setValue("Font", fontSelected);
		LOG(INFO) << "Font changed to " << fontSelected.toStdString() << " from " << oldFont.toStdString();
		emit fontChanged();
	}


	// tooltip icons
	IconSet oldIcons = (IconSet)settings.value("Icon", (char)IconSet::NYANSUKE).toInt();
	for (int i = 0; i < (char)IconSet::ICONSET_LAST_ITEM; ++i) {
		if (iconSelectButtons[i]->isChecked()) {
			settings.setValue("Icon", i);
			if (oldIcons != (IconSet)i) {
				emit iconsChanged();
			}
			break;
		}
	}

}

void SettingsWindow::handleOKButton() {
	handleApplyButton();
	close();
}

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

void SettingsWindow::initIconsArray() {
	iconSelectButtons[(char)IconSet::MEME] = ui.optionMeme;
	iconSelectButtons[(char)IconSet::NICCAGE] = ui.optionNicCage;
	iconSelectButtons[(char)IconSet::NYANSUKE] = ui.optionNyansuke;
	iconSelectButtons[(char)IconSet::SHIBE] = ui.optionShibe;
	iconSelectButtons[(char)IconSet::SYMBOLS] = ui.optionSymbols;
	iconSelectButtons[(char)IconSet::SYMBOLS2] = ui.optionSymbols2;
}

void SettingsWindow::loadCurrSettings() {
	// icons
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	IconSet currIconSet = (IconSet)settings.value("Icon", (char)IconSet::NYANSUKE).toInt();
	iconSelectButtons[(char)currIconSet]->setChecked(true);
	QString fontSelected = settings.value("Font", "Print Clearly").toString();
	ui.fontSelect->setCurrentFont(QFont(fontSelected, ui.fontSelect->currentFont().pointSize()));
}