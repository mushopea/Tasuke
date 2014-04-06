#include "Tasuke.h"
#include "InputWindow.h"
#include "Interpreter.h"

InputWindow::InputWindow(QWidget* parent) : QWidget(parent) {
	LOG(INFO) << "InputWindow instance created";

	ui.setupUi(this);
	highlighter = new InputHighlighter(ui.lineEdit->document());
	tooltipWidget = new TooltipWidget(this);
	ui.lineEdit->installEventFilter(this);
	initAnimation();

	setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background:transparent;");
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);

	connect(ui.lineEdit, SIGNAL(textChanged()), this, SLOT(handleLineEditChanged()));
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

bool InputWindow::eventFilter(QObject* object, QEvent* event) {
    if(event->type() == QEvent::KeyPress) {
		// enter key
		QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);
		if(eventKey->key() == Qt::Key_Return) {
			handleReturnPressed();
			return true;
		}

		if (Tasuke::instance().getTaskWindow().getScreen() == 0){
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

	show(); 
	raise(); 
	activateWindow();
	animation->start();
}

void InputWindow::showAndAdd() {
	showAndCenter();
	ui.lineEdit->insertPlainText(QString("add "));
}

void InputWindow::closeAndClear() {
	hide();
	ui.lineEdit->clear();
}

void InputWindow::handleReturnPressed() {
	QString command = ui.lineEdit->toPlainText();

	if (command.isEmpty()) {
		return;
	}

	Tasuke::instance().runCommand(command);
}

void InputWindow::initAnimation() {
	animation = new QPropertyAnimation(this, "opacity"); 
	animation->setEasingCurve(QEasingCurve::OutCubic); 
	animation->setDuration(700); 
	animation->setStartValue(0.0); 
	animation->setEndValue(0.95); 
}

// Will be updated when "themes" is implemented.
void InputWindow::changeBorder(int themeNumber) {
}

// Will be updated when "themes" is implemented.
void InputWindow::changeBG(int themeNumber) {
}

void InputWindow::handleLineEditChanged() {
	QString currText = ui.lineEdit->toPlainText();

	if (currText.isEmpty()) {
		hideTooltip();
	} else {
		emit inputChanged(currText);
	}
}

void InputWindow::setOpacity(qreal value) {
	wOpacity = value;
	setWindowOpacity(value);
	update();
}

qreal InputWindow::getOpacity() const {
	return wOpacity;
}

void InputWindow::hideEvent(QHideEvent* event) {
	Q_UNUSED(event);
	hideTooltip();
}