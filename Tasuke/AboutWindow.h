//@author A0100189M

#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include "ui_AboutWindow.h"

// This is a simple window that displays brief info about Tasuke
// And credits any resources used.
// It can be accessed in the tray icon context menu.

class AboutWindow : public QDialog {
	Q_OBJECT

public:
	AboutWindow(QDialog *parent = 0);
	~AboutWindow();

public slots:
	void showAndCenter();


private:
	Ui::AboutWindow ui;
};

#endif // ABOUTWINDOW_H
