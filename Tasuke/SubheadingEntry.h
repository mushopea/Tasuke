//@author A0100189M

#ifndef SUBHEADINGENTRY_H
#define SUBHEADINGENTRY_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <glog/logging.h>
#include "ui_SubheadingEntry.h"

// Subheading entry is entered inside the listWidget inside Task Window.
// It is used to sort and organize the tasks visually for the user.

class SubheadingEntry : public QWidget {
	Q_OBJECT

public:
	SubheadingEntry(const QString& content, QWidget *parent = 0);
	~SubheadingEntry();
	void changeText(const QString& text);

public slots:
	void handleReloadTheme();

private:
	Ui::SubheadingEntry ui;
	bool connectedToSettings;
	void initUI();
	void initSettingsConnect();
};

#endif //SUBHEADINGENTRY_H
