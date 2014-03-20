#include "Tasuke.h"
#include "TaskEntry.h"

TaskEntry::TaskEntry(int id, QString description, QList<QString> tags, QDateTime start, QDateTime end, QWidget* parent) : QWidget(parent), font("Print Clearly", 20), fm(font)  {

	//initialize field elements
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



void TaskEntry::setDescription(QString des){

	//first, we have to make sure the text fits.
	//if not, we will append with ellipses ("...")

	if (fm.width(des) < TaskEntry::MAX_WIDTH_FOR_DESCRIPTION){ //the description fits into the column nicely.
		ui.description->setText(des);
	} else { //the description is too long.
		ui.description->setToolTip(des);
		ui.description->setText(fm.elidedText(des, Qt::ElideRight, MAX_WIDTH_FOR_DESCRIPTION));
	}
}

void TaskEntry::setDateTimes(QDateTime start, QDateTime end){

	if(!start.isNull()){
		QString strStartDate = start.toString("dd MMM (ddd)");
		QString strStartTime = start.toString("h:mm ap");
		ui.startDate->setText(strStartDate);
		ui.startTime->setText(strStartTime);
	}

	if(!end.isNull()){
		QString strEndDate = end.toString("dd MMM (ddd)");
		QString strEndTime = end.toString("h:mm ap");
		ui.endDate->setText(strEndDate);
		ui.endTime->setText(strEndTime);
	}
}

QString TaskEntry::createTagString(QList<QString> tags){
	QString strTags = tags[0];
	strTags.prepend("#");

	if(tags.size() > 1){
		for (int i = 1; i < tags.size(); i++){ //iterate through the list to create a string of tags
			strTags.append(QString(", #"));
			strTags.append(tags[i]);
		}
	}

	return strTags;
}

void TaskEntry::setTags(QList<QString> tags){
		QString strTags = createTagString(tags);

		if (fm.width(strTags) < TaskEntry::MAX_WIDTH_FOR_TAGS){ //the tag string fits into the column nicely.
			ui.tag->setText(strTags);
		} else { //the tag string is too long.
			ui.tag->setToolTip(strTags);
			ui.tag->setText(fm.elidedText(strTags, Qt::ElideRight, MAX_WIDTH_FOR_TAGS));
		}
}

//this function sets the respective fields in the TaskEntry widget
void TaskEntry::makeWidget(){

	//ID
	QString num;
	num.setNum(id);
	ui.ID->setText(num);

	//DESCRIPTION
	setDescription(this->description);

	//DATE & TIME
	//first, we have to convert QDateTime to string.
	setDateTimes(this->start, this->end);

	//TAGS
	if(!tags.isEmpty()){
		setTags(this->tags);
	}
}


