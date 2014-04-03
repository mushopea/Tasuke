#ifndef SUBHEADINGENTRY_H
#define SUBHEADINGENTRY_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include "ui_SubheadingEntry.h"


class SubheadingEntry : public QWidget {
	Q_OBJECT

public:
	SubheadingEntry(QWidget *parent = 0);
	~SubheadingEntry();
	void changeText(QString text);

public slots:

private:
	Ui::SubheadingEntry ui;
};

#endif //SUBHEADINGENTRY_H
