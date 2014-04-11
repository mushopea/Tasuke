#include "Tasuke.h"
#include "InputWindow.h"
#include "Interpreter.h"

//@author A0100189

InputWindow::InputWindow(QWidget* parent) : QWidget(parent), animation(this, "opacity"), errorAnimation(this, "pos") {
	LOG(INFO) << "InputWindow instance created";
	initUI();
	initWidgets();
	initAnimation();
	initUIConnect();
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
//	SHOWING AND HIDING INPUTWINDOW
// ====================================================

void InputWindow::showAndCenter() {
	LOG(INFO) << "Displaying input window";

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

void InputWindow::showAndAdd() {
	LOG(INFO) << "User has clicked \"add one\"";
	showAndCenter();
	ui.lineEdit->insertPlainText(QString("add "));
}

void InputWindow::closeAndClear() {
	hide();
	ui.lineEdit->clear();
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

	if (currText.isEmpty()) {
		hideTooltip();
	} else {
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

//===================================
// THEMING 
//===================================
void InputWindow::applyDefaultTheme() {

}

void InputWindow::applyGreenTheme() {

}

void InputWindow::applySpaceTheme() {

}

void InputWindow::applyPinkTheme() {

}

void InputWindow::applyPikaTheme() {


}

void InputWindow::applyBlueTheme() {


}

void InputWindow::applyDogeTheme() {


}