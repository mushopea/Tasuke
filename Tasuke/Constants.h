#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

#define MAXIMUM_TAGS 10

// Stacked widget page indexes
static const int TASKS_PAGE = 0;
static const int TUTORIAL_PAGE = 1;

enum InputStatus {
	SUCCESS, 
	FAILURE,
	NORMAL
};

enum Theme {
	SIMPLE,
	PINK,
	BLUESKY,
	DARK,
	GALAXY,
	DOGE
};

enum IconSet {
	NYANSUKE,
	SYMBOLS,
	SYMBOLSWHITE,
	NICCAGE,
	MEMES,
	SHIBE
};


#endif
