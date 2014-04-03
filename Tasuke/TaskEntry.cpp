#include "Tasuke.h"
#include "TaskEntry.h"

TaskEntry::TaskEntry(int id, Task t, QWidget* parent) : QWidget(parent), font("Print Clearly", 20), fm(font)  {

	// Initialize field elements
	this->id = id;
	this->task = t;

	ui.setupUi(this);
	makeWidget();

	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("background:transparent;");
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
}

TaskEntry::~TaskEntry() {

}

// Strikes out the entry as done
void TaskEntry::strikeOut() {	     
	QFont font;
	font.setFamily(QStringLiteral("Print Clearly"));
	font.setPointSize(20);
	font.setStrikeOut(true);
	ui.description->setFont(font);

	QFont font1;
	font1.setFamily(QStringLiteral("Print Clearly"));
	font1.setPointSize(20);
	font1.setStrikeOut(true);
	ui.tag->setFont(font1);

	QFont font2;
	font2.setFamily(QStringLiteral("Print Clearly"));
	font2.setPointSize(12);
	font2.setStrikeOut(true);
	ui.startDate->setFont(font2);

	QFont font3;
	font3.setFamily(QStringLiteral("Print Clearly"));
	font3.setPointSize(12);
	font3.setStrikeOut(true);
	ui.endDate->setFont(font3);

	QFont font4;
	font4.setFamily(QStringLiteral("Print Clearly"));
	font4.setPointSize(12);
	font4.setStrikeOut(true);
	ui.startTime->setFont(font4);

	QFont font5;
	font5.setFamily(QStringLiteral("Print Clearly"));
	font5.setPointSize(12);
	font5.setStrikeOut(true);
	ui.endTime->setFont(font5);
}

void TaskEntry::highlightOngoing() {
	ui.ongoingLabel->show();	
	setStyleSheet("background:transparent; color: rgb(44, 99, 0);");
}

void TaskEntry::highlightOverdue() {
	ui.overdueLabel->show();	
	setStyleSheet("background:transparent; color: rgb(166, 0, 0); ");
}

void TaskEntry::setTooltip(QString des, QDateTime start, QDateTime end) {

	QString tooltipText(des);

	if (!start.isNull()) {
		QString strStartDate = start.toString("dd MMM yyyy (ddd)");
		QString strStartTime = start.toString("h:mm ap");
		tooltipText.append("\n\n Start: \n");
		tooltipText.append(strStartDate);
		tooltipText.append(strStartTime);
	}

	if (!end.isNull()) {
		QString strEndDate = end.toString("dd MMM yyyy (ddd)");
		QString strEndTime = end.toString("h:mm ap");
		tooltipText.append("\n End: \n");
		tooltipText.append(strEndDate);
		tooltipText.append(strEndTime);
	}

	ui.description->setToolTip(tooltipText);

}

void TaskEntry::setDescription(QString des) {

	// First, we have to make sure the text fits.
	// If not, we will append with ellipses ("...")

	if (fm.width(des) < TaskEntry::MAX_WIDTH_FOR_DESCRIPTION){ // The description fits into the column nicely.
		ui.description->setText(des);
	} else { // The description is too long.
		ui.description->setText(fm.elidedText(des, Qt::ElideRight, MAX_WIDTH_FOR_DESCRIPTION));
	}
}

void TaskEntry::setDateTimes(QDateTime start, QDateTime end) {

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

QString TaskEntry::createTagString(QList<QString> tags) {
	QString strTags = tags[0];
	strTags.prepend("#");

	if (tags.size() > 1) {
		for (int i = 1; i < tags.size(); i++) { // Iterate through the list to create a string of tags
			strTags.append(QString(", #"));
			strTags.append(tags[i]);
		}
	}

	return strTags;
}

void TaskEntry::setTags(QList<QString> tags) {
	QString strTags = createTagString(tags);

	if (fm.width(strTags) < TaskEntry::MAX_WIDTH_FOR_TAGS) { // The tag string fits into the column nicely.
		ui.tag->setText(strTags);
	} else { // The tag string is too long.
		ui.tag->setToolTip(strTags);
		ui.tag->setText(fm.elidedText(strTags, Qt::ElideRight, MAX_WIDTH_FOR_TAGS));
	}
}

// This function sets the respective fields in the TaskEntry widget
void TaskEntry::makeWidget() {
	QString num;
	num.setNum(this->id);
	ui.ID->setText(num);

	setDescription(task.getDescription());

	setDateTimes(task.getBegin(), task.getEnd());

	if (!task.getTags().isEmpty()) {
		setTags(task.getTags());
	}

	setTooltip(task.getDescription(), task.getBegin(), task.getEnd());

	ui.overdueLabel->hide();
	ui.ongoingLabel->hide();
}
