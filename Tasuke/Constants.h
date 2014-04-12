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

// The time interval in minutes used to calculate nextFreeTime
static const int INTERVAL = 15;

// Stacked widget page indexes
static const int TASKS_PAGE = 0;
static const int TUTORIAL_PAGE = 1;

enum class InputStatus : char {
	SUCCESS, 
	FAILURE,
	NORMAL
};

enum class Theme : char {
	DEFAULT,
	GREEN,
	SPACE,
	PINK,
	PIKACHU,
	BLUE,
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

// Icon locations
static const char* NYANSUKE_NORMAL = ":Images/images/icons/nyansukenormal.png";
static const char* NYANSUKE_SUCCESS = ":Images/images/icons/nyansukesuccess.png";
static const char* NYANSUKE_FAILURE = ":Images/images/icons/nyansukefailure.png";
static const char* SYMBOL_NORMAL = ":Images/images/icons/symbolnormal.png";
static const char* SYMBOL_SUCCESS = ":Images/images/icons/symbolsuccess.png";
static const char* SYMBOL_FAILURE = ":Images/images/icons/symbolfailure.png";
static const char* SYMBOL2_NORMAL = ":Images/images/icons/symbolnormal.png";
static const char* SYMBOL2_SUCCESS = ":Images/images/icons/symbolwhitesuccess.png";
static const char* SYMBOL2_FAILURE = ":Images/images/icons/symbolwhitefailure.png";
static const char* NICCAGE_NORMAL = ":Images/images/icons/niccagenormal.png";
static const char* NICCAGE_SUCCESS = ":Images/images/icons/niccagesuccess.png";
static const char* NICCAGE_FAILURE = ":Images/images/icons/niccagefailure.png";
static const char* MEME_NORMAL = ":Images/images/icons/memenormal.png";
static const char* MEME_SUCCESS = ":Images/images/icons/memesuccess.png";
static const char* MEME_FAILURE = ":Images/images/icons/memefailure.png";
static const char* SHIBE_NORMAL = ":Images/images/icons/dogenormal.png";
static const char* SHIBE_SUCCESS = ":Images/images/icons/dogesuccess.png";
static const char* SHIBE_FAILURE = ":Images/images/icons/dogefailure.png";


#endif
