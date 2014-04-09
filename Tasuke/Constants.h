#ifndef CONSTANTS_H
#define CONSTANTS_H

// Maximum number of tags a task can have
static const int MAXIMUM_TAGS = 10;

// These are magic numbers that are used in the modular
// arithmetic in Task::getTimeDifferenceString().
static const int MONTHS_IN_YEAR = 12;
static const int WEEKS_IN_MONTH = 4;
static const int DAYS_IN_WEEK = 7;
static const int HOURS_IN_DAY = 24;
static const int MINUTES_IN_HOUR = 60;
static const int SECONDS_IN_MINUTE = 60;
static const int MSECS_IN_SECOND = 1000;

// Stacked widget page indexes
static const int TASKS_PAGE = 0;
static const int TUTORIAL_PAGE = 1;

enum class InputStatus : char {
	SUCCESS, 
	FAILURE,
	NORMAL
};

enum class Theme : char {
	SIMPLE,
	PINK,
	BLUESKY,
	DARK,
	GALAXY,
	DOGE,
	THEME_LAST_ITEM
};

enum class IconSet : char {
	NYANSUKE,
	SYMBOLS,
	SYMBOLS2,
	NICCAGE,
	MEME,
	SHIBE,
	ICONSET_LAST_ITEM
};


#endif
