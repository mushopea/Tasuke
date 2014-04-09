#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include "ui_AboutWindow.h"

//@author A0100189

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
