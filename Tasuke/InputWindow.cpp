#include <glog/logging.h>
#include <QApplication>
#include <QDesktopWidget>
#include "Tasuke.h"
#include "InputWindow.h"

#include <QGraphicsOpacityEffect>

InputWindow::InputWindow(QWidget* parent) : QWidget(parent) {
	LOG(INFO) << "InputWindow instance created";

	ui.setupUi(this);
	highlighter = new InputHighlighter(ui.lineEdit->document());

	setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background:transparent;");
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);

	ui.lineEdit->installEventFilter(this);


}

InputWindow::~InputWindow() {
	LOG(INFO) << "InputWindow instance destroyed";
}


bool InputWindow::eventFilter(QObject* object, QEvent* event) {
    if(event->type() == QEvent::KeyPress) {
		//enter key
		QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);
		if(eventKey->key() == Qt::Key_Return) {
			handleReturnPressed();
			handleEditingFinished();
			return true;
		}

		//scroll keys
		if(eventKey->modifiers() & Qt::CTRL) {
			if(eventKey->key() == Qt::Key_Down) {
				Tasuke::instance().getTaskWindow().pageDown();
				return true;
			}

			if (eventKey->key() == Qt::Key_Up) {
				Tasuke::instance().getTaskWindow().pageUp();
				return true;
			}
		}

		if(eventKey->key() == Qt::Key_Down) {
			Tasuke::instance().getTaskWindow().scrollDown();
			return true;
		}

		if (eventKey->key() == Qt::Key_Up) {
			Tasuke::instance().getTaskWindow().scrollUp();
			return true;
		}

		//undo keys
		if (eventKey->matches(QKeySequence::Undo)) {
			Tasuke::instance().runCommand(QString("undo"));
		}

		if (eventKey->matches(QKeySequence::Redo)) {
			Tasuke::instance().runCommand(QString("redo"));
		}

		//paste shortcut
		if (eventKey->matches(QKeySequence::Paste)) {
			if(ui.lineEdit->toPlainText().trimmed().isEmpty()) {
				ui.lineEdit->insertPlainText(QString("add "));
			}
		}

    }

	if(event->type() == QEvent::FocusOut) {
	   handleEditingFinished();	
    }

    return QObject::eventFilter(object, event);
}


void InputWindow::showAndCenter() {
	LOG(INFO) << "Displaying input window";

	QPoint pos = QApplication::desktop()->screen()->rect().center() - rect().center();
	if(Tasuke::instance().getTaskWindow().isVisible()){ //if taskWindow is open
		pos.setY(Tasuke::instance().getTaskWindow().y() + Tasuke::instance().getTaskWindow().height() + 3); //set commandbox below taskWindow
		pos.setX(Tasuke::instance().getTaskWindow().x());
	} else {
		pos.setY(QApplication::desktop()->screen()->rect().height() / 4);
	}

	move(pos);

	//show();
	//raise();

	QGraphicsOpacityEffect* fade_effect = new QGraphicsOpacityEffect(this);
	this->setGraphicsEffect(fade_effect);
	QPropertyAnimation *animation = new QPropertyAnimation(fade_effect, "opacity");
	animation->setEasingCurve(QEasingCurve::OutCubic);
	animation->setDuration(700);
	animation->setStartValue(0.0);      
	animation->setEndValue(1.0);
	animation->start();
    this->setVisible(true);
	activateWindow();

}

//will be updated when "themes" is implemented.
void InputWindow::changeBorder(int themeNumber){
	QPixmap pxr(QString::fromUtf8("InputWindowMask.png"));
	QPixmap pxr2(QString::fromUtf8("InputWindowMask.png"));
	pxr.fill(Qt::red);
	pxr.setMask( pxr2.createMaskFromColor( Qt::transparent ) );


    ui.label_2->setPixmap(pxr);
}

//will be updated when "themes" is implemented.
void InputWindow::changeBG(int themeNumber){
	//ui.label_2->setPixmap(pxr);
}

void InputWindow::handleReturnPressed() {
	QString command = ui.lineEdit->toPlainText();

	if (command.isEmpty()) {
		return;
	}

	Tasuke::instance().runCommand(command);
}

void InputWindow::handleEditingFinished() {
	hide();
	ui.lineEdit->clear();
}