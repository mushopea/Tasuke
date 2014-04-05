#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <glog/logging.h>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include "ui_SettingsWindow.h"


class SettingsWindow : public QWidget {
	Q_OBJECT

public:
	SettingsWindow(QWidget *parent = 0);
	~SettingsWindow();
	void showAndCenter();

public slots:
	void changeTabs();

protected:
	bool eventFilter(QObject* object, QEvent* event);

private slots:


private:
	Ui::SettingsWindow ui;

};

#endif // SETTINGSWINDOW_H
