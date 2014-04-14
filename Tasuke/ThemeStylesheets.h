#include <QStringList>

// Stores all the stylesheets for themeing. 
// To add a new stylesheet, simply insert the stylesheet into each QStringList.
// Each theme stylesheet has the same index across all QStringLists.

class ThemeStylesheets {
public:
	static QStringList TASKWINDOW_STYLES;
	static QStringList TASKENTRY_NORMAL_STYLES;
	static QStringList TASKENTRY_SELECT_STYLES;
	static QStringList INPUTWINDOW_STYLES;
	static QStringList SUBHEADING_STYLES;

	ThemeStylesheets();
	~ThemeStylesheets();
};