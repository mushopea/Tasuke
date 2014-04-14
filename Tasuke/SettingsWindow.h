//@author A0100189M

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <glog/logging.h>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include "Constants.h"
#include "ui_SettingsWindow.h"

// The settings window handles changing of settings inside the settings window.
// Users can enable/disable features, change icon set, change font and change the theme.

class SettingsWindow : public QWidget {
	Q_OBJECT

public:
	SettingsWindow(QWidget *parent = 0);
	~SettingsWindow();

	void showAndCenter();
	void changeTabs();

signals:
	void featuresChanged();
	void themeChanged();
	void iconsChanged();
	void fontChanged();

public slots:
	void handleApplyButton();
	void handleOKButton();

protected:
	bool eventFilter(QObject* object, QEvent* event) override;
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

	// Helper functions to load current checked boxes
	void loadCurrSettings();
	void loadCurrFeatures();
	void loadCurrFont();
	void loadCurrIcons();
	void loadCurrTheme();

	// Handle changing of settings
	void editRunOnStartup();
	void editFeatures();
	void editFont();
	void editIcons();
	void editTheme();
};

#endif // SETTINGSWINDOW_H
