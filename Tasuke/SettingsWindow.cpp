#include "Tasuke.h"
#include "SettingsWindow.h"

/* TO DO 
- connect enable/disable features
- connect hotkey OK button
*/

/* DONE
- hotkey reset button. clears the field.
*/

SettingsWindow::SettingsWindow(QWidget* parent) : QWidget(parent) {
	LOG(INFO) << "SettingsWindow instance created";

	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint | Qt::Tool);

	//setAttribute(Qt::WA_TranslucentBackground);
	//setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
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

bool SettingsWindow::eventFilter(QObject* object, QEvent* event) {
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);

		if (eventKey->key() == Qt::Key_Tab){
			changeTabs();
			return true;
		}
	}
	return QObject::eventFilter(object, event);
}