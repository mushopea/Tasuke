#include "Tasuke.h"
#include "TaskWindow.h"


TaskWindow::TaskWindow(QWidget* parent) : QMainWindow(parent) {
	LOG(INFO) << "TaskWindow instance created";
	
	ui.setupUi(this);
	this->installEventFilter(this);
	
	currentlySelected = 0;
	initTut(); // Initialize tutorial window


	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
}

TaskWindow::~TaskWindow() {
	LOG(INFO) << "TaskWindow instance destroyed";
}

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
void TaskWindow::addListItem(TaskEntry* entry, int row, int pixmapID) {
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

void TaskWindow::highlightCurrentlySelected(int prevSize) {

	if ((isInRange(previouslySelected)) && (prevSize!=0)) { // Dehighlight if previous state is not empty
		Task t2 = currentTasks[previouslySelected];
		TaskEntry * entry2 = createEntry(t2, previouslySelected+1);
		addListItem(entry2, previouslySelected, 2);
		ui.taskList->takeItem(previouslySelected+1);
	}

	// Highlight currently selected
	if(isInRange(currentlySelected)) {
		Task t = currentTasks[currentlySelected];
		TaskEntry * entry = createEntry(t, currentlySelected+1);
		addListItem(entry, currentlySelected, 1);
		ui.taskList->takeItem(currentlySelected+1);
	}
}

void TaskWindow::highlightAndAnimateCurrentlySelected(int prevSize) {

	if ((isInRange(previouslySelected)) && (prevSize!=0)) { // Dehighlight if previous state is not empty
		Task t2 = currentTasks[previouslySelected];
		TaskEntry * entry2 = createEntry(t2, previouslySelected+1);
		addListItem(entry2, previouslySelected, 2);
		ui.taskList->takeItem(previouslySelected+1);
	}

	// Highlight currently selected
	if(isInRange(currentlySelected)) {
		Task t = currentTasks[currentlySelected];
		TaskEntry * entry = createEntry(t, currentlySelected+1);
		addListItem(entry, currentlySelected, 1);

		//FIGURING OUT ANIMATION

		/*QGraphicsOpacityEffect * fadeEffect = new QGraphicsOpacityEffect(ui.taskList->itemWidget(listItem)); 
		this->setGraphicsEffect(fadeEffect);
		QPropertyAnimation * animation = new QPropertyAnimation(fadeEffect, "opacity"); 
		animation->setEasingCurve(QEasingCurve::OutCubic); 
		animation->setDuration(1000); 
		animation->setStartValue(0.0); 
		animation->setEndValue(1.0); 
		animation->start();*/

		ui.taskList->takeItem(currentlySelected+1);
	}
}


void TaskWindow::focusOnNewTask(bool haveAnimation) {
	// Set indexes of selected tasks
	previouslySelected = currentlySelected;
	currentlySelected = currentTasks.size()-1; // Change this when fazli give updated task

	// Highlight newly selected task, dehighlight ex task
	if (haveAnimation) { // For add, edit, redo
		highlightAndAnimateCurrentlySelected(previousSize);
	} else { // Delete, undo
		highlightCurrentlySelected(previousSize);
	}

	// Scroll to it!
	ui.taskList->scrollToItem(ui.taskList->item(currentlySelected));
}

void TaskWindow::showTasks(QList<Task> tasks) {
	previousSize = currentTasks.size(); // Size of previous list
	currentTasks = tasks; // Update current tasks
	ui.taskList->clear();

	for (int i = 0; i < tasks.size(); i++){
		Task t = tasks[i];
		TaskEntry * entry = createEntry(t, i+1);
		addListItem(entry);
	}

	if (previousSize < currentTasks.size()) { // If an entry was deleted
		focusOnNewTask(false); // Highlight updated task
	} else {
		focusOnNewTask(true); // Highlight and animate updated task
	}
}

void TaskWindow::initTut() {
	tut = new TutorialWidget(this);
	ui.stackedWidget->addWidget(tut);
}

void TaskWindow::scrollUp() {
	if (currentlySelected>0) {
		previouslySelected = currentlySelected;
		currentlySelected--;
		ui.taskList->scrollToItem(ui.taskList->item(currentlySelected));
		highlightCurrentlySelected(currentTasks.size());
	}
}

void TaskWindow::scrollDown() {
	if(currentlySelected < ui.taskList->count()-1){
		previouslySelected = currentlySelected;
		currentlySelected++;
		ui.taskList->scrollToItem(ui.taskList->item(currentlySelected));
		highlightCurrentlySelected(currentTasks.size());
	}
}

void TaskWindow::pageUp() {
	if (currentlySelected < 1){
		return;
	}

	if (currentlySelected > TASKS_PER_PAGE-1) {
		previouslySelected = currentlySelected;
		currentlySelected -= TASKS_PER_PAGE;
	} else {
		previouslySelected = currentlySelected;
		currentlySelected = 0;
	}

	ui.taskList->scrollToItem(ui.taskList->item(currentlySelected));
	highlightCurrentlySelected(currentTasks.size());

}

void TaskWindow::pageDown() {
	if (currentlySelected == ui.taskList->count()-1) {
		return;
	}

	if (currentlySelected < ui.taskList->count() - TASKS_PER_PAGE - 1){
		previouslySelected = currentlySelected;
		currentlySelected += TASKS_PER_PAGE;
	} else {
		previouslySelected = currentlySelected;
		currentlySelected = ui.taskList->count()-1;
	}

	ui.taskList->scrollToItem(ui.taskList->item(currentlySelected));
	highlightCurrentlySelected(currentTasks.size());
}


// Returns 0 if task list is shown, 1 if tutorial is shown
int TaskWindow::getScreen() {
	return ui.stackedWidget->currentIndex();
}

void TaskWindow::changeTutorialWidgetTabs(){
	tut->changeTabs();
}

void TaskWindow::showAndMoveToSide() {
	QPoint center = QApplication::desktop()->screen()->rect().center() - rect().center();
	center.setY(QApplication::desktop()->screen()->rect().height() / 9);

	move(center);

	showNormal();
	raise();
	activateWindow();
	setFocus();
}

void TaskWindow::showListWidget() {
	ui.stackedWidget->slideInIdx(0);
}

void TaskWindow::showTutorialWidget() {
	ui.stackedWidget->slideInIdx(1);
}


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
