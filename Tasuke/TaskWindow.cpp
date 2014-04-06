#include "Tasuke.h"
#include "TaskWindow.h"
#include "Constants.h"
#include "SubheadingEntry.h"


TaskWindow::TaskWindow(QWidget* parent) : connectedToSettings(false), currentlySelectedTask(-1), onlyShowDone(false), QMainWindow(parent) {
	LOG(INFO) << "TaskWindow instance created";

	ui.setupUi(this);
	ui.backButton->hide();
	this->installEventFilter(this);

	resetSubheadingIndexes();
	initTutorial(); 
	initAnimation();
	initProgressBar();

	connect(ui.emptyAddTaskButton, SIGNAL(pressed()), this, SLOT(handleEmptyAddTaskButton()));
	connect(ui.backButton, SIGNAL(released()), this, SLOT(handleBackButton()));

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
}

TaskWindow::~TaskWindow() {
	LOG(INFO) << "TaskWindow instance destroyed";
}

//========================================
// TASK DISPLAY AND SELECTION
//=========================================

// This function is for Tasuke to get TaskWindow to highlight a particular task.
void TaskWindow::highlightTask(int taskID) {
	assert(isInRange(taskID)); 
	updateCurrentlySelectedTo(taskID);
	jumpToCurrentlySelectedTask();
}

// This function is responsible for showing all the tasks entries and subheadings.
void TaskWindow::showTasks(const QList<Task>& tasks, const QString& title) {

	previousSize = currentTasks.size(); // Size of previous list
	currentTasks = tasks; // Update current tasks
	changeTitle(title); // Change title scope
	
	onlyShowDone = title.compare("done") == 0;
	displayTaskList();

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

void TaskWindow::gotoPreviousSection() {
	char thisSection = -1, prevSection = -1;
	for (char i = (char)SubheadingType::SUBHEADING_TYPE_LAST_ITEM - 1; i >= 0; --i) {
		if (subheadingRowIndexes[i] != -1 && subheadingRowIndexes[i] <= currentlySelectedTask) {
			if (thisSection == -1) {
				thisSection = i;
			} else if (prevSection == -1) {
				prevSection = i;
				break;
			}
		}
	}
	highlightTask(prevSection == -1 ? 0 : subheadingRowIndexes[prevSection]);
	ui.taskList->scrollToItem(ui.taskList->item(getTaskEntryRow(currentlySelectedTask) - 1));
}

void TaskWindow::gotoNextSection() {
	char nextSection = -1;
	for (char i = 0; i < (char)SubheadingType::SUBHEADING_TYPE_LAST_ITEM; ++i) {
		if (subheadingRowIndexes[i] != -1 && subheadingRowIndexes[i] > currentlySelectedTask) {
			if (nextSection == -1) {
				nextSection = i;
				break;
			}
		}
	}
	highlightTask(nextSection == -1 ? currentTasks.count() - 1 : subheadingRowIndexes[nextSection]);
	ui.taskList->scrollToItem(ui.taskList->item(getTaskEntryRow(currentlySelectedTask) - 1));
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
// SLOTS
//=========================================

void TaskWindow::showAndMoveToSide() {

	if (!connectedToSettings) {
		connectedToSettings = true;
		connect(&Tasuke::instance().getSettingsWindow(), SIGNAL(fontChanged()), this, SLOT(displayTaskList()));
		LOG(INFO) << "Connected TaskWindow to SettingsWindow";
	}

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
			switch (eventKey->key()) {
				case Qt::Key::Key_Up:
					if (eventKey->modifiers() & Qt::Modifier::CTRL) {
						pageUp();
					} else if (eventKey->modifiers() & Qt::Modifier::SHIFT) {
						gotoPreviousSection();
					} else {
						scrollUp();
					}
					return true;
				case Qt::Key::Key_Down:
					if (eventKey->modifiers() & Qt::Modifier::CTRL) {
						pageDown();
					} else if (eventKey->modifiers() & Qt::Modifier::SHIFT) {
						gotoNextSection();
					} else {
						scrollDown();
					}
					return true;
				// Search backspace to go back
				case Qt::Key::Key_Backspace:
					handleBackButton();
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

void TaskWindow::initProgressBar() {
	progressBar = new QProgressBar(this);
	progressBar->setObjectName(QStringLiteral("progressBar"));
    progressBar->setGeometry(QRect(260, 230, 331, 23));
	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
}

void TaskWindow::setOpacity(qreal value) {
	wOpacity = value;
	setWindowOpacity(value);
	update();
}

qreal TaskWindow::getOpacity() const {
	return wOpacity;
}


//========================================
// PRIVATE HELPER TASK DISPLAY FUNCTIONS
//=========================================

// Displays current tasks
void TaskWindow::displayTaskList() {
	LOG(INFO) << "Displaying task list";
	ui.taskList->clear(); // Clear previous list
	resetSubheadingIndexes(); // Reset subheadings
	for (int i = 0; i < currentTasks.size(); i++) {
		displayAndUpdateSubheadings(i);
		displayTask(currentTasks[i]);
		progressBar->setValue((int)(i * 100 / (currentTasks.size() - 1)));	
	}
	
	hideProgressBarWhenDone();
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
	listItem->setSizeHint(entry->size());
	ui.taskList->insertItem(row, listItem);
	ui.taskList->setItemWidget(listItem, entry);
}

// Adds a new QListWidgetItem
void TaskWindow::addListItem(TaskEntry* entry) {
	QListWidgetItem *listItem = new QListWidgetItem();
	listItem->setSizeHint(entry->size());
	ui.taskList->addItem(listItem);
	ui.taskList->setItemWidget(listItem, entry);
}

// Displays a task entry on the list.
void TaskWindow::displayTask(const Task& t) {
	if (onlyShowDone) { // Showing done tasks
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
	for (int i = 0; i < (char)SubheadingType::SUBHEADING_TYPE_LAST_ITEM; ++i) {
		if (subheadingRowIndexes[i] != -1 && subheadingRowIndexes[i] <= taskRow) {
			++result;
		}
	}
	return result;
}

//============================================
// PRIVATE HELPER SUBHEADING DISPLAY FUNCTIONS
//============================================

void TaskWindow::resetSubheadingIndexes() {
	for (int i = 0; i < (char)SubheadingType::SUBHEADING_TYPE_LAST_ITEM; ++i) {
		subheadingRowIndexes[i] = -1;
	}
}

void TaskWindow::displayAndUpdateSubheadings(int index) {
	if (currentTasks[index].isOverdue()) {
		if (subheadingRowIndexes[(char)SubheadingType::OVERDUE] == -1) {
			subheadingRowIndexes[(char)SubheadingType::OVERDUE] = index;
			displaySubheading("Overdue tasks");
		}
	} else if (currentTasks[index].isDueToday()) {
		if (subheadingRowIndexes[(char)SubheadingType::DUE_TODAY] == -1) {
			subheadingRowIndexes[(char)SubheadingType::DUE_TODAY] = index;
			displaySubheading("Today's tasks");
		}
	} else if (!currentTasks[index].getBegin().isNull() || !currentTasks[index].getEnd().isNull()) {
		if (subheadingRowIndexes[(char)SubheadingType::TIMED] == -1) {
			subheadingRowIndexes[(char)SubheadingType::TIMED] = index;
			displaySubheading("Timed tasks");
		}
	} else {
		if (subheadingRowIndexes[(char)SubheadingType::FLOATING] == -1) {
			subheadingRowIndexes[(char)SubheadingType::FLOATING] = index;
			displaySubheading("Untimed tasks");
		}
	}
}

void TaskWindow::displaySubheading(const QString& content) {
	SubheadingEntry * subheading = new SubheadingEntry(content, this);
	QListWidgetItem *listItem = new QListWidgetItem();
	listItem->setSizeHint(subheading->size());
	ui.taskList->addItem(listItem);
	ui.taskList->setItemWidget(listItem, subheading);
}

//================================================
// PRIVATE HELPER WINDOW CONTENT DISPLAY FUNCTIONS
//================================================

// This function decides the content to show on the TaskWindow
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

// Changes title text on top
void TaskWindow::changeTitle(const QString& title) {
	if (!title.isEmpty()) {
		ui.taskScope->setText("Viewing " + title);
	}
}

// Hides progress bar and displays the task list when all tasks are loaded.
void TaskWindow::hideProgressBarWhenDone() {
	if (progressBar->value() != progressBar->maximum()) {
		ui.taskList->hide();
	} else { 
		ui.taskList->show();
		progressBar->hide();
	}
}

//=============================================
// PRIVATE HELPER SCROLLING/HIGHLIGHT FUNCTIONS
//=============================================

// Checks if a given index is in range of the task list.
bool TaskWindow::isInRange(int taskID) const {
	return (taskID >= 0) && (taskID < currentTasks.size());
}

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