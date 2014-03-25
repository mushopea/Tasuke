#include "Tasuke.h"
#include "TaskWindow.h"

TaskWindow::TaskWindow(QWidget* parent) : QMainWindow(parent) {
	LOG(INFO) << "TaskWindow instance created";

	ui.setupUi(this);
	this->installEventFilter(this);

	currentlySelected = 0;
	initTutorial(); // Initialize tutorial window
	initAnimation();

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
}

TaskWindow::~TaskWindow() {
	LOG(INFO) << "TaskWindow instance destroyed";
}

//========================================
// TASK ENTRY FUNCTIONS
//=========================================

// Checks if a given index is in range of the task list.
bool TaskWindow::isInRange(int index) {
	return (index >= 0) && (index < currentTasks.size());
}

// Creates and returns a new task entry.
TaskEntry* TaskWindow::createEntry(Task t, int index) {
	TaskEntry* entry = new TaskEntry(index, t.getDescription(), t.getTags(), t.getBegin(), t.getEnd(), this);

	if (t.isDone()) {
		entry->strikeOut();
	}

	if (t.isOverdue()) {
		entry->highlightOverdue();
	}

	return entry;
}

// Add a QListWidgetItem in a specified row with a specified background.
void TaskWindow::addListItemToRow(TaskEntry* entry, int row, int pixmapID) {
	QPixmap pxr("roundedEntryMaskSelect.png"); // Highlighted bg image
	QPixmap pxr2("roundedEntryMask.png"); // Normal bg image

	if (pixmapID == 1) {
		entry->ui.bg->setPixmap(pxr);
	} 

	if (pixmapID == 2) {
		entry->ui.bg->setPixmap(pxr2);
	}

	QListWidgetItem *listItem = new QListWidgetItem();
	listItem->setSizeHint(QSize(TASK_ENTRY_WIDTH, TASK_ENTRY_HEIGHT));
	ui.taskList->insertItem(row, listItem);
	ui.taskList->setItemWidget(listItem, entry);
}

// Adds a new QListWidgetItem
void TaskWindow::addListItem(TaskEntry* entry) {
	QListWidgetItem *listItem = new QListWidgetItem();
	listItem->setSizeHint(QSize(TASK_ENTRY_WIDTH, TASK_ENTRY_HEIGHT));
	ui.taskList->addItem(listItem);
	ui.taskList->setItemWidget(listItem, entry);
}

//========================================
// TASK DISPLAY AND SELECTION
//=========================================

// This function updates the latest selected task.
void TaskWindow::updateCurrentlySelectedTo(int row) {
	previouslySelected = currentlySelected;
	currentlySelected = row;
}

// This function will scroll to, and highlight, the currently selected task.
void TaskWindow::jumpToCurrentlySelectedTask() {
	ui.taskList->scrollToItem(ui.taskList->item(currentlySelected));
	highlightCurrentlySelectedTask(currentTasks.size());
}

// This function is for Tasuke to get TaskWindow to highlight a particular task.
void TaskWindow::highlightTask(int row) {
	if (isInRange(row)) {
		updateCurrentlySelectedTo(row);
		jumpToCurrentlySelectedTask();
	}
}

// This function highlights the selected row and dehighlights the previously highlighted.
void TaskWindow::highlightCurrentlySelectedTask(int prevSize) {
	// Dehighlight if previous state is not empty
	if ((isInRange(previouslySelected)) && (prevSize!=0)) { 
		Task t2 = currentTasks[previouslySelected];
		TaskEntry * entry2 = createEntry(t2, previouslySelected+1);
		addListItemToRow(entry2, previouslySelected, 2);
		ui.taskList->takeItem(previouslySelected+1);
	}

	// Highlight currently selected
	if(isInRange(currentlySelected)) {
		Task t = currentTasks[currentlySelected];
		TaskEntry * entry = createEntry(t, currentlySelected+1);
		addListItemToRow(entry, currentlySelected, 1);
		ui.taskList->takeItem(currentlySelected+1);
	}
}

// This function is responsible for showing all the tasks entries.
void TaskWindow::showTasks(QList<Task> tasks) {
	previousSize = currentTasks.size(); // Size of previous list
	currentTasks = tasks; // Update current tasks
	ui.taskList->clear();

	for (int i = 0; i < tasks.size(); i++){
		Task t = tasks[i];
		TaskEntry * entry = createEntry(t, i+1);
		addListItem(entry);
	}
}

//========================================
// SCROLLING
//=========================================

void TaskWindow::scrollUp() {
	if (currentlySelected>0) {
		updateCurrentlySelectedTo(currentlySelected-1);
		jumpToCurrentlySelectedTask();
	}
}

void TaskWindow::scrollDown() {
	if(currentlySelected < ui.taskList->count()-1){
		updateCurrentlySelectedTo(currentlySelected+1);
		jumpToCurrentlySelectedTask();
	}
}

void TaskWindow::pageUp() {
	if (currentlySelected < 1){
		return;
	}

	if (currentlySelected > TASKS_PER_PAGE-1) {
		updateCurrentlySelectedTo(currentlySelected-TASKS_PER_PAGE);
	} else {
		updateCurrentlySelectedTo(0);
	}

	jumpToCurrentlySelectedTask();
}

void TaskWindow::pageDown() {
	if (currentlySelected == ui.taskList->count()-1) {
		return;
	}

	if (currentlySelected < ui.taskList->count() - TASKS_PER_PAGE - 1){
		updateCurrentlySelectedTo(currentlySelected+TASKS_PER_PAGE);
	} else {
		updateCurrentlySelectedTo(ui.taskList->count()-1);
	}

	jumpToCurrentlySelectedTask();
}

//========================================
// WINDOW SHOWING
//=========================================

void TaskWindow::showAndMoveToSide() {
	QPoint center = QApplication::desktop()->screen()->rect().center() - rect().center();
	center.setY(QApplication::desktop()->screen()->rect().height() / 9);

	move(center);
	setWindowOpacity(0);
	showNormal();
	raise();
	setWindowOpacity(100);
	animation->start();
}

//========================================
// STACKED WIDGET FUNCTIONS
//=========================================

// Returns 0 if task list is shown, 1 if tutorial is shown
int TaskWindow::getScreen() {
	return ui.stackedWidget->currentIndex();
}

void TaskWindow::changeTutorialWidgetTabs(){
	tutorial->changeTabs();
}

void TaskWindow::showListWidget() {
	ui.stackedWidget->slideInIdx(0);
}

void TaskWindow::showTutorialWidget() {
	ui.stackedWidget->slideInIdx(1);
}

//========================================
// EVENTS
//=========================================


void TaskWindow::closeEvent(QCloseEvent *event) {
	hide();
	event->ignore();
}

void TaskWindow::mousePressEvent(QMouseEvent *event) {
	mpos = event->pos();
}

void TaskWindow::mouseMoveEvent(QMouseEvent *event) {
	if (event->buttons() && Qt::LeftButton) {
		QPoint diff = event->pos() - mpos;
		QPoint newpos = this->pos() + diff;

		this->move(newpos);
	}
}

bool TaskWindow::eventFilter(QObject* object, QEvent* event) {
	if (event->type() == QEvent::KeyPress) {	
		QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);

		// Scroll task list
		if (ui.stackedWidget->currentIndex() == 0) { // Is on task list

			if (eventKey->modifiers() & Qt::CTRL) {
				if (eventKey->key() == Qt::Key_Down) {
					pageDown();
					return true;
				}

				if (eventKey->key() == Qt::Key_Up) {
					pageUp();
					return true;
				}
			}

			if (eventKey->key() == Qt::Key_Down) {
				scrollDown();
				return true;
			}

			if (eventKey->key() == Qt::Key_Up) {
				scrollUp();
				return true;
			}

			// Undo and redo shortcuts
			if (eventKey->matches(QKeySequence::Undo)) {
				Tasuke::instance().runCommand(QString("undo"));
				return true;
			}

			if (eventKey->matches(QKeySequence::Redo)) {
				Tasuke::instance().runCommand(QString("redo"));
				return true;
			}
		}

		if (ui.stackedWidget->currentIndex() == 1) { // Is on tutorial
			if (eventKey->key() == Qt::Key_Tab){
				changeTutorialWidgetTabs();
				return true;
			}
		}
	}

	return QObject::eventFilter(object, event);
}

//========================================
// INITIALIZATION
//=========================================

void TaskWindow::initTutorial() {
	tutorial = new TutorialWidget(this);
	ui.stackedWidget->addWidget(tutorial);
}

void TaskWindow::initAnimation() {
	fadeEffect = new QGraphicsOpacityEffect(this); 
	this->setGraphicsEffect(fadeEffect);
	animation = new QPropertyAnimation(fadeEffect, "opacity"); 
	animation->setEasingCurve(QEasingCurve::Linear); 
	animation->setDuration(400); 
	animation->setStartValue(0.0); 
	animation->setEndValue(1.0); 
	//QObject::connect(animation, SIGNAL(valueChanged(QVariant)), this, SLOT(repaint()));
}