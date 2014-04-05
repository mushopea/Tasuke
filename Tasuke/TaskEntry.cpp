#include "Tasuke.h"
#include "TaskEntry.h"
#include "Constants.h"


TaskEntry::TaskEntry(const Task& t, QWidget* parent) : QWidget(parent), font("Print Clearly", 20), fm(font), task(t)  {

	// Initialize field elements
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
	ui.description->setStyleSheet("background:transparent; color: rgb(44, 115, 0); ");
	ui.startDate->setStyleSheet("background:transparent; color: rgb(44, 115, 0); ");
	ui.endDate->setStyleSheet("background:transparent; color: rgb(44, 115, 0); ");
	ui.startTime->setStyleSheet("background:transparent; color: rgb(44, 115, 0); ");
	ui.endTime->setStyleSheet("background:transparent; color: rgb(44, 115, 0); ");
	ui.tag->setStyleSheet("background:transparent; color: rgb(44, 115, 0); ");
	ui.ID->setStyleSheet("background:transparent; color: rgb(44, 115, 0); ");
}

void TaskEntry::highlightOverdue() {
	ui.overdueLabel->show();	
	ui.description->setStyleSheet("background:transparent; color: rgb(166, 0, 0); ");
	ui.startDate->setStyleSheet("background:transparent; color: rgb(166, 0, 0); ");
	ui.endDate->setStyleSheet("background:transparent; color: rgb(166, 0, 0); ");
	ui.startTime->setStyleSheet("background:transparent; color: rgb(166, 0, 0); ");
	ui.endTime->setStyleSheet("background:transparent; color: rgb(166, 0, 0); ");
	ui.tag->setStyleSheet("background:transparent; color: rgb(166, 0, 0); ");
	ui.ID->setStyleSheet("background:transparent; color: rgb(166, 0, 0); ");
}

void TaskEntry::setTooltip(const QString& des, const QDateTime& start, const QDateTime& end, const QList<QString>& tags) {

	// description
	QString tooltipText(des);
	tooltipText.prepend("Task description: ");

	// start datetime
	if (!start.isNull()) {
		QString strStartDate = start.toString("dd MMMM yyyy (dddd)");
		QString strStartTime = start.toString("h:mm ap");
		tooltipText.append("\n\nStart: \n");
		tooltipText.append(strStartDate);
		tooltipText.append("\n");
		tooltipText.append(strStartTime);
	}

	// end datetime
	if (!end.isNull()) {
		QString strEndDate = end.toString("dd MMMM yyyy (dddd)");
		QString strEndTime = end.toString("h:mm ap");
		tooltipText.append("\n\nEnd: \n");
		tooltipText.append(strEndDate);
		tooltipText.append("\n");
		tooltipText.append(strEndTime);
		
		// due in
		int days = QDateTime::currentDateTime().daysTo(end);
		tooltipText.append("\n\nDue in: ");
		tooltipText.append(QString::number(days));
		tooltipText.append(" days");
		// to do: get countdown/get days ago
	}

	// tags
	if (!tags.isEmpty()) {
		tooltipText.append("\n\nTagged with: ");
		tooltipText.append(createTagString(tags));
	}

	this->setToolTip(tooltipText);
}

void TaskEntry::setDescription(const QString& des) {

	// First, we have to make sure the text fits.
	// If not, we will append with ellipses ("...")

	if (fm.width(des) < TaskEntry::MAX_WIDTH_FOR_DESCRIPTION){ // The description fits into the column nicely.
		ui.description->setText(des);
	} else { // The description is too long.
		ui.description->setText(fm.elidedText(des, Qt::ElideRight, MAX_WIDTH_FOR_DESCRIPTION));
	}
}

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

QString TaskEntry::createTagString(const QList<QString>& tags) {
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

void TaskEntry::setTags(const QList<QString>& tags) {
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
	ui.ID->setText(QString::number(task.getId()+1));

	setDescription(task.getDescription());

	setDateTimes(task.getBegin(), task.getEnd());

	if (!task.getTags().isEmpty()) {
		setTags(task.getTags());
	}

	setTooltip(task.getDescription(), task.getBegin(), task.getEnd(), task.getTags());

	ui.overdueLabel->hide();
	ui.ongoingLabel->hide();
}
