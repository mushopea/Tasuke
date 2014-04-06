#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

#define MAXIMUM_TAGS 10

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
