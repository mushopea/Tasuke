#include "Tasuke.h"
#include "SubheadingEntry.h"

SubheadingEntry::SubheadingEntry(QWidget *parent): QWidget(parent) {
	ui.setupUi(this);

	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
}

SubheadingEntry::~SubheadingEntry() {

}

void SubheadingEntry::changeText(QString text) {
	ui.content->setText(text);
}