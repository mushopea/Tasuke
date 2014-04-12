#include <QSettings>
#include "Tasuke.h"
#include "TaskEntry.h"
#include "Constants.h"

//@author A0100189

// Each TaskEntry object represents a Task with different ID, description, dates and tags.
// They are entered into the listWidget in TaskWindow.

TaskEntry::TaskEntry(const Task& t, QWidget* parent) : QWidget(parent), task(t)  {
	initUI();
	initLabelsArray();
	initFonts();
	makeWidget();
}

TaskEntry::~TaskEntry() {

}

// ================================================
// FUNCTIONS THAT SET LOOK OF TASK ENTRY
// ================================================

// Highlight ongoing task in green
void TaskEntry::highlightOngoing() {
	ui.ongoingLabel->show();	
	ui.description->setStyleSheet("background:transparent; color: rgb(40, 155, 36); ");
	ui.startDate->setStyleSheet("background:transparent; color: rgb(40, 155, 36); ");
	ui.endDate->setStyleSheet("background:transparent; color: rgb(40, 155, 36); ");
	ui.startTime->setStyleSheet("background:transparent; color: rgb(40, 155, 36); ");
	ui.endTime->setStyleSheet("background:transparent; color: rgb(40, 155, 36); ");
	ui.tag->setStyleSheet("background:transparent; color: rgb(40, 155, 36); ");
	ui.ID->setStyleSheet("background:transparent; color: rgb(40, 155, 36); ");
}

// Highlight overdue task in red
void TaskEntry::highlightOverdue() {
	ui.overdueLabel->show();	
	ui.description->setStyleSheet("background:transparent; color: rgb(210, 44, 44); ");
	ui.startDate->setStyleSheet("background:transparent; color: rgb(210, 44, 44); ");
	ui.endDate->setStyleSheet("background:transparent; color: rgb(210, 44, 44); ");
	ui.startTime->setStyleSheet("background:transparent; color: rgb(210, 44, 44); ");
	ui.endTime->setStyleSheet("background:transparent; color: rgb(210, 44, 44); ");
	ui.tag->setStyleSheet("background:transparent; color: rgb(210, 44, 44); ");
	ui.ID->setStyleSheet("background:transparent; color: rgb(210, 44, 44); ");
}

// ================================================
// FUNCTIONS TO SET FIELD CONTENT OF TASK ENTRY
// ================================================

// Generates a string of #tags
QString TaskEntry::createTagString(const QList<QString>& tags) const {
	QString strTags = tags[0];
	strTags.prepend("#");
	if (tags.size() > 1) {
		for (int i = 1; i < tags.size(); i++) { // Iterate through the list to create a string of tags
			assert(!tags[i].isEmpty());
			strTags.append(", #");
			strTags.append(tags[i]);
		}
	}
	return strTags;
}

// Sets task ID
void TaskEntry::setID(int ID) {
	assert(ID>=0);
	ui.ID->setText(QString::number(task.getId() + 1));
}

// Set tooltip
void TaskEntry::setTooltip(const QString& description, const QDateTime& start, const QDateTime& end, 
						   const QList<QString>& tags, const QString& dueInMessage) {
	// description
	QString tooltipText(description);
	tooltipText.prepend("Task description: ");
	// start datetime
	if (!start.isNull()) {
		tooltipText.append("\n\nStart: \n" + start.toString("dd MMMM yyyy (dddd)\nh:mm ap"));
	}
	// end datetime
	if (!end.isNull()) {
		tooltipText.append("\n\nEnd: \n" + end.toString("dd MMMM yyyy (dddd)\nh:mm ap"));
		tooltipText.append("\n\n" + dueInMessage);
	}
	// tags
	if (!tags.isEmpty()) {
		tooltipText.append("\n\nTagged with: ");
		tooltipText.append(createTagString(tags));
	}
	// Set it!
	this->setToolTip(tooltipText);
}

// Set description that cannot be empty
void TaskEntry::setDescription(const QString& description) {
	assert(!description.isEmpty());
	ui.description->setText(ui.description->fontMetrics().elidedText(description, Qt::ElideRight, 
							ui.description->contentsRect().width()));
}

// Set the date time in the specific format
void TaskEntry::setDateTimes(const QDateTime& start, const QDateTime& end) {
	if (!start.isNull()) {
		QString strStartDate = start.toString("dd MMM (ddd)");
		QString strStartTime = start.toString("h:mm ap");
		ui.startDate->setText(strStartDate);
		ui.startTime->setText(strStartTime);
	}
	if (!end.isNull()) {
		QString strEndDate = end.toString("dd MMM (ddd)");
		QString strEndTime = end.toString("h:mm ap");
		ui.endDate->setText(strEndDate);
		ui.endTime->setText(strEndTime);
	}
}

// Set the string of tags
void TaskEntry::setTags(const QList<QString>& tags) {
	assert(!task.getTags().isEmpty());

	QString strTags = createTagString(tags);
	ui.tag->setText(ui.tag->fontMetrics().elidedText(strTags, Qt::ElideRight, ui.tag->width()));
}

// ================================================
// INITIALIZATIONS
// ================================================

void TaskEntry::initUI() {
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
	ui.overdueLabel->hide();
	ui.ongoingLabel->hide();
}

void TaskEntry::initLabelsArray() {
	labels[(char)TaskEntryLabel::DESCRIPTION] = ui.description;
	labels[(char)TaskEntryLabel::END_DATE] = ui.endDate;
	labels[(char)TaskEntryLabel::END_TIME] = ui.endTime;
	labels[(char)TaskEntryLabel::ID] = ui.ID;
	labels[(char)TaskEntryLabel::START_DATE] = ui.startDate;
	labels[(char)TaskEntryLabel::START_TIME] = ui.startTime;
	labels[(char)TaskEntryLabel::TAG] = ui.tag;
}

// Sets the font of the fields in task entry
void TaskEntry::initFonts() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	QString fontFamily = settings.value("Font", "Print Clearly").toString();

	for (int i = 0; i < (char)TaskEntryLabel::TASKENTRYLABEL_LAST_ITEM; ++i) {
		QFont font = labels[i]->font();
		font.setFamily(fontFamily);
		if (font.family().compare("Print Clearly") != 0) {
			if ((i == (char)TaskEntryLabel::DESCRIPTION) || (i == (char)TaskEntryLabel::ID) || (i == (char)TaskEntryLabel::TAG)) {
				font.setPointSize(font.pointSize() - FONT_SIZE_DIFF);
			}
		}
		labels[i]->setFont(font);
	}
}

// This function sets the respective fields in the TaskEntry widget
void TaskEntry::makeWidget() {
	setID(task.getId());
	setDescription(task.getDescription());
	setDateTimes(task.getBegin(), task.getEnd());
	if (!task.getTags().isEmpty()) {
		setTags(task.getTags());
	}
	setTooltip(task.getDescription(), task.getBegin(), task.getEnd(), task.getTags(), task.getTimeDifferenceString());
}
