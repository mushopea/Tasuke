#ifndef SUBHEADINGENTRY_H
#define SUBHEADINGENTRY_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <glog/logging.h>
#include "ui_SubheadingEntry.h"

//@author A0100189

class SubheadingEntry : public QWidget {
	Q_OBJECT

public:
	SubheadingEntry(const QString& content, QWidget *parent = 0);
	~SubheadingEntry();
	void changeText(const QString& text);

public slots:

private:
	Ui::SubheadingEntry ui;
};

#endif //SUBHEADINGENTRY_H
