#ifndef SYSTEMTRAYWIDGET_H
#define SYSTEMTRAYWIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>

//@author A0096836M

// Widget for tray icon in the system tray. It is used to display info and for
// user menu
class SystemTrayWidget : public QWidget {
	Q_OBJECT

public:
	SystemTrayWidget(QWidget *parent = 0);
	~SystemTrayWidget();

	void showMessage(QString message);

private slots:
	void handleShowTaskWindow();
	void handleShowInputWindow();
	void handleShowTutorialWidget();
	void handleShowSettingsWindow();
	void handleShowAboutWindow();
	void handleIconActivated(QSystemTrayIcon::ActivationReason reason);
	void handleAboutToQuit();

private:
	QSystemTrayIcon* trayIcon;

	void installTrayIcon();
};

#endif // SYSTEMTRAYWIDGET_H
