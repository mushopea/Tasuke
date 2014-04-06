#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

#define MAXIMUM_TAGS 10

#define MONTHS_IN_YEAR 12
#define WEEKS_IN_MONTH 4
#define DAYS_IN_WEEK 7
#define HOURS_IN_DAY 24
#define MINUTES_IN_HOUR 60
#define SECONDS_IN_MINUTE 60
#define MILLISECONDS_IN_SECOND 1000

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
	DOGE
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
