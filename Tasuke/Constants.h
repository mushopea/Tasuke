#ifndef CONSTANTS_H
#define CONSTANTS_H

//@author A0096836M

#include <QStringList>
#include <QList>
#include <QTime>
#include <QDate>

// General app metadata
const char* const TASUKE = "Tasuke";
const char* const NAME_ORGANIZATION = TASUKE;
const char* const NAME_APPLICATION = TASUKE;
const char* const SHARED_MEMORY_KEY = TASUKE;

//@author A0096863M

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

//@author A0100189

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

//@author A0096836M

// Log messages
const char* const MSG_STARTING_UP = "Tasuke starting up!";
const char* const MSG_ALREADY_RUNNING =  "Tasuke is already running! Open Tasuke using ctrl+space";
const char* const MSG_TASUKE_CREATED = "Tasuke object created";
const char* const MSG_TASUKE_DESTROYED = "Tasuke object destroyed";
const char* const MSG_LOADING_DICTIONARY = "Loading dictionary";
const char* const MSG_LOADING_FONTS = "Loading fonts";
const char* const MSG_STORAGE_CHANGED = "Storage changed";
const char* const MSG_COMMAND_STACK_PUSH =  "Pushing command to history stack";
const char* const MSG_UNDO = "Undoing command";
const char* const MSG_NO_UNDO ="Nothing to undo";
const char* const MSG_REDO = "Redoing command";
const char* const MSG_NO_REDO = "Nothing to redo";
const char* const MSG_HOTKEYMANAGER_CREATED = "HotKeyManager created";
const char* const MSG_HOTKEYMANAGER_DESTROYED = "HotKeyManager destroyed";

// Macros for log messages
#define MSG_ERROR_PARSING(message) \
	"Error parsing command" << QString(message).toStdString()
#define MSG_SHOWING_MESSAGE(message) \
	"Showing message: " << QString(message).toStdString()
#define MSG_UPDATING_TASKWINDOW(tasks) \
	"Updating task window with " << QString(tasks).toStdString() << " tasks"
#define MSG_HIGHLIGHT_TASK(id) \
	"Highlighting task with id  " << id
#define MSG_INTERPRETTING(command) \
	"Interpretting " << command.toStdString()
#define MSG_HOTKEY_PRESSED(key) \
	"Hot key pressed with keycode " << QString::number((int)key).toStdString()

// Filenames of dictionary files
const char* const SPELL_GB_DICFILE = "en_GB.dic";
const char* const SPELL_GB_AFFFILE = "en_GB.aff";
const char* const SPELL_US_DICFILE = "en_US.dic";

// The relative resource path for a mac app bundle
const char* const MAC_RESOURCE_PATH = "/../Resources/";

// The string name of the TRY_RESULT metatype
const char* const METATYPE_TRY_RESULT = "TRY_RESULT";
const char* const METATYPE_KEYCOMIBNATION = "KeyCombination";

// Lists of words to add into dictionary
const QStringList SPELL_NONWORD_COMMANDS = QStringList() << "rm" << "ls"
	<< "nd";
const QStringList SPELL_MONTH_NAMES = QStringList() << "january" << "february"
	<< "march" << "april" << "may" << "june" << "july" << "august"
	<< "september" << "october" << "november" << "december";
const QStringList SPELL_MONTH_NAMES_SHORT = QStringList() << "jan" << "feb"
	<< "mar" << "apr" << "jun" << "jul" << "aug" << "sep" << "sept" << "oct"
	<< "nov" << "dec";
const QStringList SPELL_DAY_NAMES = QStringList() << "monday" << "tuesday"
	<< "wednesday" << "thursday" << "friday" << "saturday" << "sunday";
const QStringList SPELL_DAY_NAMES_SHORT = QStringList() << "mon" << "tue" << "tues"
	<< "wed" << "thu" << "thur" << "fri" << "sat" << "sun";
const QList<QStringList> SPELL_INCLUDE_LISTS = QList<QStringList>()
	<< SPELL_NONWORD_COMMANDS << SPELL_MONTH_NAMES << SPELL_MONTH_NAMES_SHORT
	<< SPELL_DAY_NAMES << SPELL_DAY_NAMES_SHORT;

// List of font locations
const QStringList FONT_LIST = QStringList() << ":/Fonts/fonts/Quicksand_Book.otf"
	<< ":/Fonts/fonts/Quicksand_Book_Oblique.otf"
	<< ":/Fonts/fonts/Quicksand_Light.otf"
	<< ":/Fonts/fonts/Quicksand_Light_Oblique.otf"
	<< ":/Fonts/fonts/Quicksand_Bold.otf"
	<< ":/Fonts/fonts/Quicksand_Bold_Oblique.otf"
	<< ":/Fonts/fonts/PrintClearly.otf"
	<< ":/Fonts/fonts/PrintBold.otf"
	<< ":/Fonts/fonts/Consolas.ttf";

// Command keywords
const char* const COMMAND_NIL = "";
const char* const COMMAND_ADD = "add";
const char* const COMMAND_EDIT = "edit";
const char* const COMMAND_REMOVE = "remove";
const char* const COMMAND_SHOW = "show";
const char* const COMMAND_HIDE = "hide";
const char* const COMMAND_DONE = "done";
const char* const COMMAND_UNDONE = "undone";
const char* const COMMAND_UNDO = "undo";
const char* const COMMAND_REDO = "redo";
const char* const COMMAND_CLEAR = "clear";
const char* const COMMAND_HELP = "help";
const char* const COMMAND_ABOUT = "about";
const char* const COMMAND_SETTINGS = "settings";
const char* const COMMAND_EXIT = "exit";

// List of command keywords
const QStringList COMMANDS = QStringList() << COMMAND_ADD << COMMAND_EDIT
	<< COMMAND_REMOVE << COMMAND_SHOW << COMMAND_HIDE << COMMAND_DONE
	<< COMMAND_UNDONE << COMMAND_UNDO << COMMAND_REDO << COMMAND_CLEAR
	<< COMMAND_HELP << COMMAND_ABOUT << COMMAND_SETTINGS << COMMAND_EXIT;

// Command formats
const char* const FORMAT_ALL = "add | edit | done | undone | remove "
	"| show | undo | redo | settings | help | exit";
const char* const FORMAT_ADD =
	"add {description}[my task]{/description} {tag}#tag{/tag}";
const char* const FORMAT_ADD_PERIOD = 
	"add {description}[my task]{/description} from {date}{start}[start]{/star}"
	" to {end}[end]{/end}{/date} {tag}#tag{/tag}";
const char* const FORMAT_ADD_DEADLINE =
	"add {description}[my task]{/description} by/on/at {date}{end}[end]{/end}"
	"{/date} {tag}#tag{/tag}";
const char* const FORMAT_REMOVE = 
	"remove {id}[task no]{/id} | remove [task no], [task no], ... | "
	"remove [task no] - [task no]";
const char* const FORMAT_EDIT = 
	"edit {id}[task no]{/id} {description}[thing to change] "
	"-[thing to remove]{/description}";
const char* const FORMAT_DONE = 
	"done {id}[task no]{/id} | done [task no], [task no], ... | "
	"done [task no] - [task no]";
const char* const FORMAT_UNDONE = 
	"undone {id}[task no]{/id} | undone [task no], [task no], ... | "
	"undone [task no] - [task no]";
const char* const FORMAT_SHOW = 
	"show [keyword] | done | undone | overdue | ongoing | today | tomorrow";
const char* const FORMAT_HIDE = "hide";
const char* const FORMAT_UNDO = "undo {times}[times]{/times} | max";
const char* const FORMAT_REDO = "redo {times}[times]{/times} | max";
const char* const FORMAT_CLEAR = "clear";
const char* const FORMAT_HELP = "help";
const char* const FORMAT_SETTINGS = "settings";
const char* const FORMAT_ABOUT = "about";
const char* const FORMAT_EXIT = "exit";

// Command descriptions
const char* const DESCRIPTION_ALL = "Use one of these keywords to begin";
const char* const DESCRIPTION_ADD = "Adds a simple task.";
const char* const DESCRIPTION_ADD_PERIOD = "Adds a task with a time period.";
const char* const DESCRIPTION_ADD_DEADLINE = "Adds a task with a deadline.";
const char* const DESCRIPTION_REMOVE = "Removes task(s) from the list.";
const char* const DESCRIPTION_EDIT = "Edits existing task.";
const char* const DESCRIPTION_DONE = "Marks tasks as done.";
const char* const DESCRIPTION_UNDONE = "Marks tasks as undone.";
const char* const DESCRIPTION_SHOW = "Shows certain tasks.";
const char* const DESCRIPTION_HIDE = "Hides the task list.";
const char* const DESCRIPTION_UNDO = "Undos your last command(s).";
const char* const DESCRIPTION_REDO = "Redos your last command(s).";
const char* const DESCRIPTION_CLEAR = "Clears all tasks in your list.";
const char* const DESCRIPTION_HELP = "Shows the tutorial.";
const char* const DESCRIPTION_SETTINGS = "Open the settings window.";
const char* const DESCRIPTION_ABOUT = "Shows about Tasuke.";
const char* const DESCRIPTION_EXIT = "Exits the program.";

// Some command regex
const QRegExp ADD_DEADLINE_REGEX = QRegExp("\\b(by|at|on)\\b");
const QRegExp ADD_PREIOD_REGEX = QRegExp("\\bfrom\\b");
const QRegExp BRACE_REGEX = QRegExp("\\{(.*)\\}");

// HTML markup for tooltip widget
const char* const HTML_ERROR_BEGIN = "<font color='#FA7597'>";
const char* const HTML_ERROR_END = "</font>";
const char* const HTML_FORMAT_BEGIN = "<font color='#999'>";
const char* const HTML_FORMAT_END = "</font>";
const char* const HTML_DESCRIPTION_BEGIN = "<br<<font color='white'>";
const char* const HTML_DESCRIPTION_END = "</font>";

// Delimiters for interpreter
const char* const DELIMITER_AT = "@";
const char* const DELIMITER_HASH = "#";
const char* const DELIMITER_DASH = "-";
const char* const DELIMITER_DASH_AT = "-@";
const char* const DELIMITER_DASH_HASH = "-#";
const char* const DELIMITER_COMMA = ",";

// Delimiters in char form
const char CHAR_DELIMITER_AT = '@';
const char CHAR_DELIMITER_HASH = '#';
const char CHAR_DELIMITER_DASH = '-';

// List of delimiters
const QStringList DELIMITERS = QStringList() << DELIMITER_AT << DELIMITER_HASH
	<< DELIMITER_DASH;

// Keywords for interpreter
const char* const KEYWORD_NIL = "";
const char* const KEYWORD_DONE = "done";
const char* const KEYWORD_UNDONE = "undone";
const char* const KEYWORD_ONGOING = "ongoing";
const char* const KEYWORD_OVERDUE = "overdue";
const char* const KEYWORD_TODAY = "today";
const char* const KEYWORD_TOMORROW = "tomorrow";
const char* const KEYWORD_ALL = "all";
const char* const KEYWORD_EVERYTHING = "everything";
const char* const KEYWORD_MAX = "max";
const char* const KEYWORD_LAST = "last";
const char* const KEYWORD_BACKSLASH = "\\";

// Titles for task view
const char* const TITLE_DONE = "done tasks";
const char* const TITLE_UNDONE = "undone tasks";
const char* const TITLE_ONGOING = "ongoing tasks";
const char* const TITLE_OVERDUE = "overdue tasks";
const char* const TITLE_TODAY = "tasks due today";
const char* const TITLE_TOMORROW = "tasks due tomorrow";

// Error descriptions
const char* const ERROR_MULTIPLE_DATES =
	"You can't have more than 2 time periods or deadlines in a task.";
const char* const ERROR_TAG_NO_NAME = "Please give a name for your tag.";
const char* const ERROR_TAG_REMOVE_NO_NAME =
	"You need to tell me what tag you want to remove";
const char* const ERROR_DONT_UNDERSTAND = "I don't undesrtand this command.";
const char* const ERROR_ADD_EMPTY = "You need to tell me what to add.";
const char* const ERROR_NO_DESCRIPTION = 
	"You need to give a description to your task.";
const char* const ERROR_REMOVE_NO_ID = 
	"You need to tell me which task(s) to remove.";
const char* const ERROR_EDIT_NO_ID = "You need to tell me which task to edit.";
const char* const ERROR_EDIT_EMPTY = 
	"You need to tell me what you want to change for the task.";
const char* const ERROR_DONE_NO_ID = 
	"You need to tell me what to mark as done.";
const char* const ERROR_UNDONE_NO_ID = 
	"You need to tell me what to mark as undone.";
const char* const ERROR_NO_LAST = "There is no last task.";
const char* const ERROR_NO_ID = "You need to give me a task number.";
const char* const ERROR_DATE_BEGIN =
	"Please give me a valid start time for this task.";
const char* const ERROR_DATE_END = 
	"Please give me a valid deadline for this task.";

// Macros for error messages
#define ERROR_DATE_INVALID_PERIOD(timePeriod) \
	QString("Start time (%1) is after end time (%2).")\
	.arg(timePeriod.begin.toString(), timePeriod.end.toString())
#define ERROR_DATE_NO_A_RANGE(range) \
	QString("'%1' doesn't look like a valid date range.").arg(range)
#define ERROR_ID_NO_A_RANGE(range) \
	QString("'%1' doesn't look like a valid range.").arg(range)
#define ERROR_ID_INVALID_RANGE(begin, end) \
	QString("Start number (%1) is after end number (%2).").arg(begin, end)
#define ERROR_ID_OUT_OF_RANGE(id, numTasks) \
	QString("There is no task '%1'." \
	"Please give a number between 1 and %2.")\
	.arg(QString::number(id), QString::number(numTasks))
#define ERROR_NOT_A_NUMBER(number) \
	QString("'%1' doesn't look like a number.").arg(number)
#define ERROR_DONT_KNOW(what) \
	QString("I don't know what to do for '%1'").arg(what)

const char* const EXCEPTION_NULL_PTR = "attempt to dereference null pointer";
const char* const EXCEPTION_NOT_IMPLEMENTED = "not implemented";
const char* const EXCEPTION_NO_MORE_TASKS = "no more tasks in the list";
const char* const EXCEPTION_ICONSET_OUT_OF_RANGE = "out of range icon set was stored and attempted access in settings.";
const char* const EXCEPTION_THEME_OUT_OF_RANGE = "out of range theme was stored and attempted access in settings.";

// Error location in format
const char* const WHERE_DATE = "date";
const char* const WHERE_TAG = "tag";
const char* const WHERE_DESCRIPTION = "description";
const char* const WHERE_ID = "id";
const char* const WHERE_TIMES = "times";
const char* const WHERE_BEGIN = "begin";
const char* const WHERE_END = "end";

// Time constants
const QTime TIME_BEFORE_MIDNIGHT = QTime(23,59);
const QTime TIME_MIDNIGHT = QTime(0,0);
const char* const TIME_AM = "am";
const char* const TIME_PM = "pm";
const char* const TIME_FORMAT = "hh:mm ap";
const int YEARS_MILIENIUM = 2000;
const int YEARS_CENTURY = 100;
const char* const DATE_FORMAT = "dd/MM/yyyy";

// Regex for removing things when parsing dates
const QList<QRegExp> REMOVE_DATE_REGEX = QList<QRegExp>() 
	<< QRegExp(",\\b") << QRegExp("\\bthis\\b") << QRegExp("\\bnext\\b")
	<< QRegExp("\\bnthe\\b");

// Regex to match named days
const QRegExp DAY_2DAY_REGEX = QRegExp("\\b2day\\b");
const QRegExp DAY_TMR_REGEX = QRegExp("\\btmr\\b");
const QRegExp DAY_TML_REGEX = QRegExp("\\btml\\b");
const QRegExp DAY_MON_REGEX = QRegExp("\\bmon\\b");
const QRegExp DAY_TUE_REGEX = QRegExp("\\btue\\b");
const QRegExp DAY_TUES_REGEX = QRegExp("\\btues\\b");
const QRegExp DAY_WED_REGEX = QRegExp("\\bwed\\b");
const QRegExp DAY_THU_REGEX = QRegExp("\\bthu\\b");
const QRegExp DAY_THUR_REGEX = QRegExp("\\bthur\\b");
const QRegExp DAY_THURS_REGEX = QRegExp("\\bthurs\\b");
const QRegExp DAY_FRI_REGEX = QRegExp("\\bfri\\b");
const QRegExp DAY_SAT_REGEX = QRegExp("\\bsat\\b");
const QRegExp DAY_SUN_REGEX = QRegExp("\\bsun\\b");

// Regex to match special days
const QRegExp DAY_YESTERDAY_REGEX = QRegExp("\\byesterday\\b");
const QRegExp DAY_TODAY_REGEX = QRegExp("\\btoday\\b");
const QRegExp DAY_TOMORROW_REGEX = QRegExp("\\btomorrow\\b");
const QRegExp DAY_AFTER_TOMORROW_REGEX = QRegExp("\\bday after tomorrow\\b");

// Macros for special days that relative to current time
#define DATE_YESTERDAY \
	QDate::currentDate().addDays(-1).toString(DATE_FORMAT)
#define DATE_TODAY \
	QDate::currentDate().toString(DATE_FORMAT)
#define DATE_TOMORROW \
	QDate::currentDate().addDays(1).toString(DATE_FORMAT)
#define DATE_AFTER_TOMORROW \
	QDate::currentDate().addDays(2).toString(DATE_FORMAT)

// Names of days
const char* const DAY_TODAY = "today";
const char* const DAY_TOMORROW = "tomorrow";
const char* const DAY_MONDAY = "monday";
const char* const DAY_TUESDAY = "tuesday";
const char* const DAY_WEDNESDAY = "wednesday";
const char* const DAY_THURSDAY = "thursday";
const char* const DAY_FRIDAY = "friday";
const char* const DAY_SATURDAY = "saturday";
const char* const DAY_SUNDAY = "sunday";

// List of names of days
const QStringList DAY_NAMES = QStringList() << DAY_MONDAY << DAY_TUESDAY
	<< DAY_WEDNESDAY << DAY_THURSDAY << DAY_FRIDAY << DAY_SATURDAY
	<< DAY_SUNDAY;

// List of named times names
const QList<QRegExp> TIME_NAMES_REGEX = QList<QRegExp>() 
	<< QRegExp("\\bdawn\\b") << QRegExp("\\bmorning\\b")
	<< QRegExp("\\bnoon\\b") << QRegExp("\\bafternoon\\b")
	<< QRegExp("\\bevening\\b") << QRegExp("\\bnight\\b")
	<< QRegExp("\\bmidnight\\b");

// List of named times
const QStringList TIME_NAMED = QStringList()
	<< QTime(6,0).toString(TIME_FORMAT)
	<< QTime(10,0).toString(TIME_FORMAT)
	<< QTime(12,0).toString(TIME_FORMAT)
	<< QTime(18,0).toString(TIME_FORMAT)
	<< QTime(20,0).toString(TIME_FORMAT)
	<< QTime(22,0).toString(TIME_FORMAT)
	<< QTime(23,59).toString(TIME_FORMAT);

// List of substitution regexes
const char* const EQUIV_AT_REPLACE = " @";
const QList<QRegExp> EQUIV_AT_REGEX = QList<QRegExp>()
	<< QRegExp("(?:\\s)by\\b") << QRegExp("(?:\\s)at\\b")
	<< QRegExp("(?:\\s)from\\b") << QRegExp("(?:\\s)on\\b");
const QList<QRegExp> EQUIV_ADD_REGEX = QList<QRegExp>()
	<< QRegExp("^do\\b") << QRegExp("^create\\b");
const QList<QRegExp> EQUIV_EDIT_REGEX = QList<QRegExp>()
	<< QRegExp("^change\\b");
const QList<QRegExp> EQUIV_REMOVE_REGEX = QList<QRegExp>()
	<< QRegExp("^rm\\b") << QRegExp("^delete\\b");
const QList<QRegExp> EQUIV_SHOW_REGEX = QList<QRegExp>()
	<< QRegExp("^ls\\b") << QRegExp("^search\\b") << QRegExp("^find\\b");
const QList<QRegExp> EQUIV_EXIT_REGEX = QList<QRegExp>()
	<< QRegExp("^quit\\b");
const QRegExp EQUIV_TO_REGEX = QRegExp("\\bto\\b");

// Special tag for format display
#define PSEUDO_TAG_BEGIN(tag) \
	"{"+QString(tag)+"}"
#define PSEUDO_TAG_END(tag) \
	"{/"+QString(tag)+"}"

#endif
