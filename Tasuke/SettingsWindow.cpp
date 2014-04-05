#include "Tasuke.h"
#include "SettingsWindow.h"
#include <QSettings>

SettingsWindow::SettingsWindow(QWidget* parent) : QWidget(parent) {
	LOG(INFO) << "SettingsWindow instance created";

	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint | Qt::Tool);
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
}

SettingsWindow::~SettingsWindow() {
	LOG(INFO) << "SettingsWindow instance destroyed";
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
	//QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	//settings.setValue("Description", tasks[i]->getDescription());
	//settings.value("Description").toString()
}

void SettingsWindow::handleOKButton() {
	handleApplyButton();
	hide();
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