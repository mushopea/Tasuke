#include "Tasuke.h"
#include "SubheadingEntry.h"

SubheadingEntry::SubheadingEntry(const QString& content, QWidget *parent): QWidget(parent) {
	ui.setupUi(this);

	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);

	changeText(content);
}

SubheadingEntry::~SubheadingEntry() {

}

void SubheadingEntry::changeText(const QString& text) {
	ui.content->setText(text);
}