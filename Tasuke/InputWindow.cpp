#include "Tasuke.h"
#include "InputWindow.h"
#include "Interpreter.h"
#include <QSettings>

//@author A0100189

// The input window is also the command box. 
// The input window handles text input, keyword highlighting and displaying of command tooltip.

InputWindow::InputWindow(QWidget* parent) : QWidget(parent), animation(this, "opacity"), 
	errorAnimation(this, "pos"), connectedToSettings(false) {
		LOG(INFO) << "InputWindow instance created";
		initUI();
		initWidgets();
		initAnimation();
		initUIConnect();
		reloadTheme();
}

InputWindow::~InputWindow() {
	LOG(INFO) << "InputWindow instance destroyed";
}

// Allows other classes to display a tooltip message relative to the status of the input.
void InputWindow::showTooltipMessage(InputStatus status, QString message) {
	if (!tooltipWidget->isVisible()) {
		tooltipWidget->showAndAlign();
	}
	tooltipWidget->setText(status, message);
}

void InputWindow::hideTooltip() {
	tooltipWidget->hide();
}

void InputWindow::doErrorAnimation() {
	errorAnimation.start();
}

// ====================================================
//	EVENTS
// ====================================================

bool InputWindow::eventFilter(QObject* object, QEvent* event) {
	if(event->type() == QEvent::KeyPress) {
		// enter key
		QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);
		if(eventKey->key() == Qt::Key_Return) {
			handleReturnPressed();
			return true;
		}

		if (Tasuke::instance().getTaskWindow().getScreen() == 0) { // On main view
			// Scroll keys for tasks
			switch (eventKey->key()) {
			case Qt::Key::Key_Up:
				if (eventKey->modifiers() & Qt::Modifier::CTRL) {
					Tasuke::instance().getTaskWindow().pageUp();
				} else if (eventKey->modifiers() & Qt::Modifier::SHIFT) {
					Tasuke::instance().getTaskWindow().gotoPreviousSection();
				} else {
					Tasuke::instance().getTaskWindow().scrollUp();
				}
				return true;
			case Qt::Key::Key_Down:
				if (eventKey->modifiers() & Qt::Modifier::CTRL) {
					Tasuke::instance().getTaskWindow().pageDown();
				} else if (eventKey->modifiers() & Qt::Modifier::SHIFT) {
					Tasuke::instance().getTaskWindow().gotoNextSection();
				} else {
					Tasuke::instance().getTaskWindow().scrollDown();
				}
				return true;
			}

			// undo keys
			if (eventKey->matches(QKeySequence::Undo)) {
				Tasuke::instance().runCommand(QString("undo"));
			}

			if (eventKey->matches(QKeySequence::Redo)) {
				Tasuke::instance().runCommand(QString("redo"));
			}

			// paste shortcut
			if (eventKey->matches(QKeySequence::Paste)) {
				if(ui.lineEdit->toPlainText().trimmed().isEmpty()) {
					ui.lineEdit->insertPlainText(QString("add "));
				}
			}
		}

		if (Tasuke::instance().getTaskWindow().getScreen() == 1) { // On tutorial window
			if (eventKey->key() == Qt::Key_Tab){
				Tasuke::instance().getTaskWindow().changeTutorialWidgetTabs();
				return true;
			}
		}
	}

	if(event->type() == QEvent::FocusOut) {
		closeAndClear();
	}

	return QObject::eventFilter(object, event);
}

void InputWindow::hideEvent(QHideEvent* event) {
	Q_UNUSED(event);
	hideTooltip();
}

// ====================================================
//	SLOTS
// ====================================================

// Shows and moves the input window to center relative to task window
void InputWindow::showAndCenter() {
	LOG(INFO) << "Displaying input window";
	initSettingsConnect();

	QPoint pos = QApplication::desktop()->screen()->rect().center() - rect().center();
	if(Tasuke::instance().getTaskWindow().isVisible()){ // If taskWindow is open
		pos.setY(Tasuke::instance().getTaskWindow().y() + Tasuke::instance().getTaskWindow().height() + 3); //set commandbox below taskWindow
		pos.setX(Tasuke::instance().getTaskWindow().x());
	} else {
		pos.setY(QApplication::desktop()->screen()->rect().height() / 4);
	}

	move(pos);
	initErrorAnimation();

	show(); 
	raise(); 
	activateWindow();
	animation.start();
}

// Shows the input window with an "Add " command 
void InputWindow::showAndAdd() {
	LOG(INFO) << "User has clicked \"add one\"";
	showAndCenter();
	ui.lineEdit->insertPlainText("add ");
}

// Closes and clears the text box
void InputWindow::closeAndClear() {
	hide();
	ui.lineEdit->clear();
}

// Reloads theme according to settings.
void InputWindow::reloadTheme() {
	LOG(INFO) << "Reloading theme in InputWindow";
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

// Enables or disables features such as highlighter, spellcheck & tooltip according to settings
void InputWindow::reloadFeatures() {
	LOG(INFO) << "Reloading highlighter in InputWindow";
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	
	bool highlightCommandsEnabled = settings.value("HighlightCommandsEnabled", true).toBool();
	highlighter->setCommandsEnabled(highlightCommandsEnabled);

	bool highlightSpellcheckEnabled = settings.value("SpellcheckEnabled", true).toBool();
	highlighter->setSpellcheckEnabled(highlightSpellcheckEnabled);

	bool tooltipEnabled = settings.value("TooltipEnabled", true).toBool();
	showTooltip = tooltipEnabled;
}


// ====================================================
//	PRIVATE SLOTS
// ====================================================

// Handles the enter button being pressed
void InputWindow::handleReturnPressed() {
	QString command = ui.lineEdit->toPlainText();
	if (command.isEmpty()) {
		return;
	}
	Tasuke::instance().runCommand(command);
}

// Handles the event when line edit changes
void InputWindow::handleLineEditChanged() {
	QString currText = ui.lineEdit->toPlainText();

	if(showTooltip) {
		if (currText.isEmpty()) {
			hideTooltip();
		} 
		emit inputChanged(currText);
	}
}

// ====================================================
//	GETTER & SETTER
// ====================================================

void InputWindow::setOpacity(qreal value) {
	wOpacity = value;
	setWindowOpacity(value);
	update();
}

qreal InputWindow::getOpacity() const {
	return wOpacity;
}

// ====================================================
//	INITIALIZATIONS
// ====================================================

// Initialise user interface
void InputWindow::initUI() {
	ui.setupUi(this);
	ui.lineEdit->installEventFilter(this);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
}

// Initialise widgets
void InputWindow::initWidgets() {
	highlighter = new InputHighlighter(ui.lineEdit->document());
	tooltipWidget = new TooltipWidget(this);
}

// Initialise connection of slots and signals
void InputWindow::initUIConnect() {
	connect(ui.lineEdit, SIGNAL(textChanged()), this, SLOT(handleLineEditChanged()));
}

// Initialize fade in animation of input window
void InputWindow::initAnimation() {
	animation.setEasingCurve(QEasingCurve::OutCubic); 
	animation.setDuration(700); 
	animation.setStartValue(0.0); 
	animation.setEndValue(1.0); 
}

// Initialize error wiggle animation of input window
void InputWindow::initErrorAnimation() {
	errorAnimation.setEasingCurve(QEasingCurve::OutElastic); 
	errorAnimation.setDuration(500);

	QPoint posBefore;
	posBefore.setY(y());
	posBefore.setX(x() - 20);

	QPoint posAfter;
	posAfter.setY(y());
	posAfter.setX(x());

	errorAnimation.setStartValue(posBefore); 
	errorAnimation.setEndValue(posAfter);
}

void InputWindow::initSettingsConnect() {
	if (!connectedToSettings) {
		connectedToSettings = true;
		connect(&Tasuke::instance().getSettingsWindow(), SIGNAL(themeChanged()), this, SLOT(reloadTheme()));
		connect(&Tasuke::instance().getSettingsWindow(), SIGNAL(featuresChanged()), this, SLOT(reloadFeatures()));
		LOG(INFO) << "Connected InputWindow to SettingsWindow";
	}
}


//===================================
// THEMING 
//===================================
void InputWindow::applyDefaultTheme() {
	setStyleSheet("QLabel#bg{border-radius: 8px; background-color: white;}"
		"QLabel#border{border-radius: 10px; background-color: rgb(74, 74, 74);}"
		"QTextEdit{background-color: transparent;}"
		"background:transparent;");
}

void InputWindow::applyGreenTheme() {
	setStyleSheet("QLabel#bg{border-radius: 8px; background-image:url(:/Images/images/theme2/bg.png);}"
		"QLabel#border{border-radius: 10px; background-color: rgb(84,117,17);}"
		"QTextEdit{background-color: transparent; color: rgb(40,40,40);}"
		"background:transparent;");
}

void InputWindow::applySpaceTheme() {
	setStyleSheet("QLabel#bg{border-radius: 8px; background-image:url(:/Images/images/theme3/bg.png);}"
		"QLabel#border{border-radius: 10px; background-color: black;}"
		"QTextEdit{background-color: transparent; color: white;}"
		"background:transparent;");
}

void InputWindow::applyPinkTheme() {
	setStyleSheet("QLabel#bg{border-radius: 8px; background-image:url(:/Images/images/theme4/bg.png);}"
		"QLabel#border{border-radius: 10px; background-color: rgb(236,169,177);}"
		"QTextEdit{background-color: transparent; color: rgb(40,40,40);}"
		"background:transparent;");
}

void InputWindow::applyPikaTheme() {
	setStyleSheet("QLabel#bg{border-radius: 8px; background-image:url(:/Images/images/theme5/bg.png);}"
		"QLabel#border{border-radius: 10px; background-color: rgb(192,150,100);}"
		"QTextEdit{background-color: transparent; color: rgb(120,89,49);}"
		"background:transparent;");
}

void InputWindow::applyBlueTheme() {
	setStyleSheet("QLabel#bg{border-radius: 8px; background-image:url(:/Images/images/theme6/bg.png);}"
		"QLabel#border{border-radius: 10px; background-color: rgb(132,174,215);}"
		"QTextEdit{background-color: transparent;color: rgb(40,40,40);}"
		"background:transparent;");
}

void InputWindow::applyDogeTheme() {
	setStyleSheet("QLabel#bg{border-radius: 8px; background-image:url(:/Images/images/theme7/inputbg.png);}"
		"QLabel#border{border-radius: 10px; background-color: rgb(255,0,222);}"
		"QTextEdit{background-color: transparent; color: rgb(255,0,0);	font: 75 25pt \"Comic Sans MS\";}"
		"background:transparent;");
}