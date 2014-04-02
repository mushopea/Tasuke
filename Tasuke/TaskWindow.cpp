#include "Tasuke.h"
#include "TaskWindow.h"

TaskWindow::TaskWindow(QWidget* parent) : QMainWindow(parent) {
	LOG(INFO) << "TaskWindow instance created";

	ui.setupUi(this);
	ui.backButton->hide();
	this->installEventFilter(this);

	currentlySelected = 1;
	initTutorial(); // Initialize tutorial window
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
bool TaskWindow::isInRange(int index) {
	return (index >= 0) && (index < currentTasks.size());
}

// Changes title text on top
void TaskWindow::changeTitle(QString title) {
	if (!title.isEmpty()) {
		ui.taskScope->setText("Viewing " + title + " tasks");
	}
}

// Creates and returns a new task entry.
TaskEntry* TaskWindow::createEntry(Task t, int index) {
	TaskEntry* entry = new TaskEntry(index, t, this);

	if (t.isOverdue()) {
		entry->highlightOverdue();
	}

	if (t.isOngoing()) {
		entry->highlightOngoing();
	}
	return entry;
}

// Add a QListWidgetItem in a specified row with a specified background.
void TaskWindow::addListItemToRow(TaskEntry* entry, int row, int pixmapID) {
	QPixmap pxr(":/Images/images/theme1/entryBgSelect.png"); // Highlighted bg image
	QPixmap pxr2(":/Images/images/theme1/entryBg.png"); // Normal bg image

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

// Displays a task entry on the list.
void TaskWindow::displayTask(Task t, int index, int showDone) {
	if (showDone == 0) { // Showing done tasks
		TaskEntry * entry = createEntry(t, index);
		addListItem(entry);
	} else { // Showing undone tasks
		if (!t.isDone()) {
			TaskEntry * entry = createEntry(t, index);
			addListItem(entry);
		}
	}
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
	assert(isInRange(row)); 
	updateCurrentlySelectedTo(row);
	jumpToCurrentlySelectedTask();
	
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
void TaskWindow::showTasks(QList<Task> tasks, QString title) {
	previousSize = currentTasks.size(); // Size of previous list
	currentTasks = tasks; // Update current tasks
	
	ui.taskList->clear(); // Clear previous list
	changeTitle(title);
	for (int i = 0; i < tasks.size(); i++) {
		displayTask(tasks[i], i+1, title.compare("done"));
	}

	decideContent(); // Show column label or 'no tasks' message.
	showBackButtonIfSearching(title);
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
	if(currentlySelected < ui.taskList->count()-1) {
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

	if (currentlySelected < ui.taskList->count() - TASKS_PER_PAGE - 1) {
		updateCurrentlySelectedTo(currentlySelected+TASKS_PER_PAGE);
	} else {
		updateCurrentlySelectedTo(ui.taskList->count()-1);
	}

	jumpToCurrentlySelectedTask();
}

//========================================
// SLOTS
//=========================================

void TaskWindow::showAndMoveToSide() {
	showListWidget(); // Shows the task list

	QPoint center = QApplication::desktop()->screen()->rect().center() - rect().center();
	center.setY(QApplication::desktop()->screen()->rect().height() / 9);

	move(center);
	setWindowOpacity(0);
	show();
	raise();
	setWindowOpacity(100);
	animation->start();
}

// Shows message when task list is empty.
void TaskWindow::handleEmptyAddTaskButton() {
	Tasuke::instance().getInputWindow().showAndAdd();
}

// Goes back to default view
void TaskWindow::handleBackButton() {
	showTasks(Tasuke::instance().getStorage().getTasks());
}

//========================================
// STACKED WIDGET FUNCTIONS
//=========================================

// Returns 0 if task list is shown, 1 if tutorial is shown
int TaskWindow::getScreen() {
	return ui.stackedWidget->currentIndex();
}

void TaskWindow::changeTutorialWidgetTabs(){
	tutorial.changeTabs();
}

void TaskWindow::showListWidget() {
	ui.stackedWidget->slideInIdx(0);
}

void TaskWindow::showTutorialWidget() {
	ui.stackedWidget->slideInIdx(1);
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
			
		}
	}

	return QObject::eventFilter(object, event);
}

//========================================
// INITIALIZATION
//=========================================

void TaskWindow::initTutorial() {
	ui.stackedWidget->addWidget(&tutorial);
	tutorial.goToFirstPage();
}

void TaskWindow::initAnimation() {
	fadeEffectThis = new QGraphicsOpacityEffect(this); 
	this->setGraphicsEffect(fadeEffectThis);
	animation = new QPropertyAnimation(fadeEffectThis, "opacity"); 
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
void TaskWindow::showBackButtonIfSearching(QString title) {
	if (!title.isEmpty()) {
		ui.backButton->show();
	} else {
		ui.backButton->hide();
	}
}
