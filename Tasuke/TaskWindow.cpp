#include "Tasuke.h"
#include "TaskWindow.h"
#include "Constants.h"
#include "SubheadingEntry.h"


TaskWindow::TaskWindow(QWidget* parent) : QMainWindow(parent) {
	LOG(INFO) << "TaskWindow instance created";

	ui.setupUi(this);
	ui.backButton->hide();
	this->installEventFilter(this);

	currentlySelectedTask = 1;

	resetSubheadingIndexes();
	initTutorial(); 
	initAnimation();

	connect(ui.emptyAddTaskButton, SIGNAL(pressed()), this, SLOT(handleEmptyAddTaskButton()));
	connect(ui.backButton, SIGNAL(released()), this, SLOT(handleBackButton()));

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
}

TaskWindow::~TaskWindow() {
	LOG(INFO) << "TaskWindow instance destroyed";
}

//========================================
// TASK DISPLAY FUNCTIONS
//=========================================

// Checks if a given index is in range of the task list.
bool TaskWindow::isInRange(int taskID) const {
	return (taskID >= 0) && (taskID < currentTasks.size());
}

// Changes title text on top
void TaskWindow::changeTitle(const QString& title) {
	if (!title.isEmpty()) {
		ui.taskScope->setText("Viewing " + title);
	}
}

// Creates and returns a new task entry.
TaskEntry* TaskWindow::createEntry(const Task& t) {
	TaskEntry* entry = new TaskEntry(t, this);

	if (t.isOverdue()) {
		entry->highlightOverdue();
	}

	if (t.isOngoing()) {
		entry->highlightOngoing();
	}
	return entry;
}

// Add a QListWidgetItem in a specified row with a specified background.
void TaskWindow::addListItemToRow(TaskEntry* entry, int row, const QString& type) {
	if (type.compare("select") == 0) {
		entry->ui.bg->setStyleSheet("border-radius: 12px; background-color: rgb(188, 188, 188);");
	} 

	if (type.compare("deselect") == 0) {
		entry->ui.bg->setStyleSheet("border-radius: 12px; background-color: rgb(203, 202, 202);");
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

void TaskWindow::displaySubheading(const QString& content) {
	SubheadingEntry * subheading = new SubheadingEntry(content, this);
	QListWidgetItem *listItem = new QListWidgetItem();
	listItem->setSizeHint(subheading->size());
	ui.taskList->addItem(listItem);
	ui.taskList->setItemWidget(listItem, subheading);
}

// Displays a task entry on the list.
void TaskWindow::displayTask(const Task& t, int showDone) {
	if (showDone == 0) { // Showing done tasks
		TaskEntry * entry = createEntry(t);
		addListItem(entry);
	} else { // Showing undone tasks
		if (!t.isDone()) {
			TaskEntry * entry = createEntry(t);
			addListItem(entry);
		}
	}
}


int TaskWindow::getTaskEntryRow(int taskRow) const {
	int result = taskRow;
	for (int i = 0; i < SubheadingType::SUBHEADING_TYPE_LAST_ITEM; ++i) {
		if (subheadingRowIndexes[i] != -1 && subheadingRowIndexes[i] <= taskRow) {
			++result;
		}
	}
	return result;
}

//========================================
// TASK DISPLAY AND SELECTION
//=========================================

// This function updates the latest selected task.
void TaskWindow::updateCurrentlySelectedTo(int taskID) {
	previouslySelectedTask = currentlySelectedTask;
	currentlySelectedTask = taskID;
}

// This function will scroll to, and highlight, the currently selected task.
void TaskWindow::jumpToCurrentlySelectedTask() {
	ui.taskList->scrollToItem(ui.taskList->item(currentlySelectedTask == 0 ? 0 : getTaskEntryRow(currentlySelectedTask)));
	highlightCurrentlySelectedTask(currentTasks.size());
}

// This function is for Tasuke to get TaskWindow to highlight a particular task.
void TaskWindow::highlightTask(int taskID) {
	assert(isInRange(taskID)); 
	updateCurrentlySelectedTo(taskID);
	jumpToCurrentlySelectedTask();
}

// This function highlights the selected row and dehighlights the previously highlighted.
void TaskWindow::highlightCurrentlySelectedTask(int prevSize) {
	// Dehighlight if previous state is not empty
	if ((isInRange(previouslySelectedTask)) && (prevSize!=0)) { 
		Task t2 = currentTasks[previouslySelectedTask];
		TaskEntry * entry2 = createEntry(t2);
		int prevSelectedRow = getTaskEntryRow(previouslySelectedTask);
		addListItemToRow(entry2, prevSelectedRow, "deselect");
		ui.taskList->takeItem(prevSelectedRow + 1);
	}

	// Highlight currently selected
	if(isInRange(currentlySelectedTask)) {
		Task t = currentTasks[currentlySelectedTask];
		TaskEntry * entry = createEntry(t);
		int currSelectedRow = getTaskEntryRow(currentlySelectedTask);
		addListItemToRow(entry, currSelectedRow, "select");
		ui.taskList->takeItem(currSelectedRow + 1);
	}
}

// This function is responsible for showing all the tasks entries and subheadings.
void TaskWindow::showTasks(const QList<Task>& tasks, const QString& title) {

	resetSubheadingIndexes();

	previousSize = currentTasks.size(); // Size of previous list
	currentTasks = tasks; // Update current tasks

	ui.taskList->clear(); // Clear previous list
	changeTitle(title);

	for (int i = 0; i < tasks.size(); i++) {
		if (tasks[i].isOverdue()) {
			if (subheadingRowIndexes[SubheadingType::OVERDUE] == -1) {
				subheadingRowIndexes[SubheadingType::OVERDUE] = i;
				displaySubheading("Overdue tasks");
			}
		} else if (tasks[i].isDueToday()) {
			if (subheadingRowIndexes[SubheadingType::DUE_TODAY] == -1) {
				subheadingRowIndexes[SubheadingType::DUE_TODAY] = i;
				displaySubheading("Today's tasks");
			}
		} else if (!tasks[i].getBegin().isNull() || !tasks[i].getEnd().isNull()) {
			if (subheadingRowIndexes[SubheadingType::TIMED] == -1) {
				subheadingRowIndexes[SubheadingType::TIMED] = i;
				displaySubheading("Timed tasks");
			}
		} else {
			if (subheadingRowIndexes[SubheadingType::FLOATING] == -1) {
				subheadingRowIndexes[SubheadingType::FLOATING] = i;
				displaySubheading("Untimed tasks");
			}
		}

		displayTask(tasks[i], title.compare("done"));
	}

	decideContent(); // Show column label or 'no tasks' message.
	showBackButtonIfSearching(title);
}


//========================================
// SCROLLING
//=========================================

void TaskWindow::scrollUp() {
	if (currentlySelectedTask > 0) {
		updateCurrentlySelectedTo(currentlySelectedTask - 1);
		jumpToCurrentlySelectedTask();
	}
}

void TaskWindow::scrollDown() {
	if(currentlySelectedTask < currentTasks.size() - 1) {
		updateCurrentlySelectedTo(currentlySelectedTask + 1);
		jumpToCurrentlySelectedTask();
	}
}

void TaskWindow::pageUp() {
	if (currentlySelectedTask < 1){
		return;
	}

	if (currentlySelectedTask > TASKS_PER_PAGE - 1) {
		updateCurrentlySelectedTo(currentlySelectedTask - TASKS_PER_PAGE);
	} else {
		updateCurrentlySelectedTo(0);
	}

	jumpToCurrentlySelectedTask();
}

void TaskWindow::pageDown() {
	if (currentlySelectedTask == currentTasks.size() - 1) {
		return;
	}

	if (currentlySelectedTask < currentTasks.size() - TASKS_PER_PAGE - 1) {
		updateCurrentlySelectedTo(currentlySelectedTask + TASKS_PER_PAGE);
	} else {
		updateCurrentlySelectedTo(currentTasks.size() - 1);
	}

	jumpToCurrentlySelectedTask();
}

//========================================
// SLOTS
//=========================================

void TaskWindow::showAndMoveToSide() {
	//if(animation->state() != QAbstractAnimation::Stopped)
	//	return;

	showListWidget(); // Shows the task list

	QPoint center = QApplication::desktop()->screen()->rect().center() - rect().center();
	center.setY(QApplication::desktop()->screen()->rect().height() / 9);

	move(center);
	animation->start();
	raise();
	show();
}

// Shows message when task list is empty.
void TaskWindow::handleEmptyAddTaskButton() {
	Tasuke::instance().getInputWindow().showAndAdd();
}

// Goes back to default view
void TaskWindow::handleBackButton() {
	showTasks(Tasuke::instance().getStorage().getTasks());	
	changeTitle("all tasks");
}

//========================================
// STACKED WIDGET FUNCTIONS
//=========================================

// Returns 0 if task list is shown, 1 if tutorial is shown
int TaskWindow::getScreen() const {
	return ui.stackedWidget->currentIndex();
}

void TaskWindow::changeTutorialWidgetTabs(){
	tutorial.changeTabs();
}

void TaskWindow::showListWidget() {
	ui.stackedWidget->slideInIdx(TASKS_PAGE);
}

void TaskWindow::showTutorialWidget() {
	ui.stackedWidget->slideInIdx(TUTORIAL_PAGE);
	tutorial.reset();
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

			// Search backspace to go back
			if (eventKey->key() == Qt::Key_Backspace) {
				handleBackButton();
				return true;
			}

		} else {
			// Tutorial shortcuts start here

			if (ui.stackedWidget->currentIndex() == 1) { // Is on tutorial
				if (eventKey->key() == Qt::Key_Tab){
					changeTutorialWidgetTabs();
					return true;
				}
			}

			// Navigate tutorial pages.
			if (eventKey->key() == Qt::Key_Right) {
				tutorial.goNextPage();
				return true;
			}

			if (eventKey->key() == Qt::Key_Left) {
				tutorial.goPrevPage();
				return true;
			}

			if (eventKey->key() == Qt::Key_Backspace) {
				showListWidget();
				return true;
			}

		}
	}

	return QObject::eventFilter(object, event);
}

//========================================
// INITIALIZATION
//=========================================


void TaskWindow::resetSubheadingIndexes(){
	for (int i = 0; i < SubheadingType::SUBHEADING_TYPE_LAST_ITEM; ++i) {
		subheadingRowIndexes[i] = -1;
	}
}

void TaskWindow::initTutorial() {
	ui.stackedWidget->addWidget(&tutorial);
	tutorial.goToFirstPage();
}

void TaskWindow::initAnimation() {
	animation = new QPropertyAnimation(this, "opacity"); 
	animation->setEasingCurve(QEasingCurve::Linear); 
	animation->setDuration(400); 
	animation->setStartValue(0.0); 
	animation->setEndValue(1.0); 
}

void TaskWindow::decideContent() {
	if (ui.taskList->count() == 0) {
		ui.emptyTaskMessage->show();
		ui.columnLabels->hide();
	} else {
		ui.emptyTaskMessage->hide();
		ui.columnLabels->show();
	}
}

// Displays the back button during search to allow user to go back to default view in a click.
void TaskWindow::showBackButtonIfSearching(const QString& title) {
	if (!title.isEmpty()) {
		ui.backButton->show();
	} else {
		ui.backButton->hide();
	}
}

void TaskWindow::setOpacity(qreal value) {
	wOpacity = value;
	setWindowOpacity(value);
	update();
}

qreal TaskWindow::getOpacity() const {
	return wOpacity;
}
