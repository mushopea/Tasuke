#include <QSettings>
#include "Tasuke.h"
#include "TaskEntry.h"
#include "Constants.h"


TaskEntry::TaskEntry(const Task& t, QWidget* parent) : QWidget(parent), task(t)  {
	// Initialize field elements
	ui.setupUi(this);
	makeWidget();

	initLabelsArray();
	initFonts();

	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("background:transparent;");
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
}

TaskEntry::~TaskEntry() {

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

void TaskEntry::initLabelsArray() {
	labels[(char)TaskEntryLabel::DESCRIPTION] = ui.description;
	labels[(char)TaskEntryLabel::END_DATE] = ui.endDate;
	labels[(char)TaskEntryLabel::END_TIME] = ui.endTime;
	labels[(char)TaskEntryLabel::ID] = ui.ID;
	labels[(char)TaskEntryLabel::START_DATE] = ui.startDate;
	labels[(char)TaskEntryLabel::START_TIME] = ui.startTime;
	labels[(char)TaskEntryLabel::TAG] = ui.tag;
}

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

	//fm = QFontMetrics(ui.description->font(), ui.description);
}

void TaskEntry::setTooltip(const QString& des, const QDateTime& start, const QDateTime& end, const QList<QString>& tags) {

	// description
	QString tooltipText(des);
	tooltipText.prepend("Task description: ");

	// start datetime
	if (!start.isNull()) {
		tooltipText.append("\n\nStart: \n" + start.toString("dd MMMM yyyy (dddd)\nh:mm ap"));
	}

	// end datetime
	if (!end.isNull()) {
		tooltipText.append("\n\nEnd: \n" + end.toString("dd MMMM yyyy (dddd)\nh:mm ap"));

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

	ui.description->setText(ui.description->fontMetrics().elidedText(des, Qt::ElideRight, ui.description->contentsRect().width()));
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

QString TaskEntry::createTagString(const QList<QString>& tags) const {
	QString strTags = tags[0];
	strTags.prepend("#");

	if (tags.size() > 1) {
		for (int i = 1; i < tags.size(); i++) { // Iterate through the list to create a string of tags
			strTags.append(", #");
			strTags.append(tags[i]);
		}
	}

	return strTags;
}

void TaskEntry::setTags(const QList<QString>& tags) {
	QString strTags = createTagString(tags);

	ui.tag->setText(ui.tag->fontMetrics().elidedText(strTags, Qt::ElideRight, ui.tag->width()));
}

// This function sets the respective fields in the TaskEntry widget
void TaskEntry::makeWidget() {
	ui.ID->setText(QString::number(task.getId() + 1));

	setDescription(task.getDescription());

	setDateTimes(task.getBegin(), task.getEnd());

	if (!task.getTags().isEmpty()) {
		setTags(task.getTags());
	}

	setTooltip(task.getDescription(), task.getBegin(), task.getEnd(), task.getTags());

	ui.overdueLabel->hide();
	ui.ongoingLabel->hide();
}
