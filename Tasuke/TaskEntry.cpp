#include "Tasuke.h"
#include "TaskEntry.h"

TaskEntry::TaskEntry(int id, QString description, QList<QString> tags, QDateTime start, QDateTime end, QWidget* parent) : QWidget(parent), font("Print Clearly", 20), fm(font)  {

	// Initialize field elements
	this->id = id;
	this->description = description;
	this->tags = tags;
	this->start = start;
	this->end = end;

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
	setStyleSheet("background:transparent; color: rgb(44, 99, 0);");
}

void TaskEntry::highlightOverdue() {
	setStyleSheet("background:transparent; color: rgb(166, 0, 0); ");
}

 

void TaskEntry::setDescription(QString des) {

	// First, we have to make sure the text fits.
	// If not, we will append with ellipses ("...")

	if (fm.width(des) < TaskEntry::MAX_WIDTH_FOR_DESCRIPTION){ // The description fits into the column nicely.
		ui.description->setText(des);
	} else { // The description is too long.
		ui.description->setToolTip(des);
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
	num.setNum(id);
	ui.ID->setText(num);

	setDescription(this->description);

	setDateTimes(this->start, this->end);

	if (!tags.isEmpty()) {
		setTags(this->tags);
	}
}
