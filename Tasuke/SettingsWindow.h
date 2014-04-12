#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <glog/logging.h>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include "ui_SettingsWindow.h"
#include "Constants.h"

//@author A0100189

class SettingsWindow : public QWidget {
	Q_OBJECT

public:
	SettingsWindow(QWidget *parent = 0);
	~SettingsWindow();
	void showAndCenter();

signals:
	void featuresChanged();
	void themeChanged();
	void iconsChanged();
	void fontChanged();

public slots:
	void changeTabs();
	void handleApplyButton();
	void handleOKButton();

protected:
	bool eventFilter(QObject* object, QEvent* event);
	virtual void showEvent(QShowEvent *event) override;

private:
	Ui::SettingsWindow ui;	
	QRadioButton * iconSelectButtons[(char)IconSet::ICONSET_LAST_ITEM];
	QRadioButton * themeSelectButtons[(char)Theme::THEME_LAST_ITEM];
	
	// Initializers
	void initUI();
	void initUIConnect();
	void initIconsArray();
	void initThemeArray();
	void loadCurrSettings();

	// Helper functions to load current checked boxes
	void loadCurrFeatures();
	void loadCurrFont();
	void loadCurrIcons();
	void loadCurrTheme();

	// Handle changed settings
	void editRunOnStartup();
	void editFeatures();
	void editFont();
	void editIcons();
	void editTheme();
};

#endif // SETTINGSWINDOW_H
