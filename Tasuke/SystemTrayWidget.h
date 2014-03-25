#ifndef SYSTEMTRAYWIDGET_H
#define SYSTEMTRAYWIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>

class SystemTrayWidget : public QWidget {
	Q_OBJECT

public:
	SystemTrayWidget(QWidget *parent = 0);
	~SystemTrayWidget();

	void showMessage(QString message);

private slots:
	void handleShowTaskWindow();
	void handleShowInputWindow();
	void handleShowAboutWindow();
	void handleIconActivated(QSystemTrayIcon::ActivationReason reason);
	void handleAboutToQuit();

private:
	QSystemTrayIcon* trayIcon;
};

#endif // SYSTEMTRAYWIDGET_H
