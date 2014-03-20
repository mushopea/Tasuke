#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QWidget>
#include "ui_AboutWindow.h"


class AboutWindow : public QDialog
{
	Q_OBJECT

public:
	AboutWindow(QDialog *parent = 0);
	~AboutWindow();

public slots:
	void showAndCenter();

protected:
	//void closeEvent(QCloseEvent* event);

private slots:


private:
	Ui::AboutWindow ui;
};

#endif // ABOUTWINDOW_H
