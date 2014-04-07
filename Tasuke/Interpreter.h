#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QMutex>
#include "Commands.h"

// This class is a factory class. It creates an instance of an ICommand from
// the string input by the user at the prompt. All ICommands produced from
// this factory must be cleaned up using delete.
class Interpreter {
private:
	typedef struct {
		QDateTime begin;
		QDateTime end;
	} TIME_PERIOD;

	static int last;

	static QStringList timeFormats;
	static QStringList dateFormatsWithoutYear;
	static QStringList dateFormats;
	static QStringList dateTimeFormatsWithoutYear;
	static QStringList dateTimeFormats;

	static QStringList timeFormatsAp;
	static QStringList dateTimeFormatsAp;
	static QStringList dateTimeFormatsWithoutYearAp;

	static bool formatsAlreadyInit;
	static QMutex mutex;
	
	static QString substitute(QString text);
	static QString substituteForTimePeriod(QString text);
	static QString substituteForDate(QString text);

	static QHash<QString, QString> decompose(QString text);
	static QString removeBefore(QString text, QString before);
	static int parseId(QString idString);
	static QList<int> parseIdList(QString idListString);
	static QList<int> parseIdRange(QString idRangeString);
	static TIME_PERIOD parseTimePeriod(QString timePeriod);
	static QDateTime parseDate(QString dateString, bool isEnd = true);
	static QDate nextWeekday(int weekday);
	static void generateTimeFormats();
	static void generateDateFormatsWithoutYear();
	static void generateDateFormats();
	static void generateDateTimeFormatsWithoutYear();
	static void generateDateTimeFormats();

	static ICommand* createAddCommand(QString commandString);
	static ICommand* createRemoveCommand(QString commandString);
	static ICommand* createEditCommand(QString commandString);
	static ICommand* createClearCommand(QString commandString);
	static ICommand* createDoneCommand(QString commandString);
	static ICommand* createUndoneCommand(QString commandString);

	static void doShow(QString commandString);
	static void doAbout();
	static void doHide();
	static void doUndo(QString commandString);
	static void doRedo(QString commandString);
	static void doHelp();
	static void doSettings();
	static void doExit();

public:	
	static void setLast(int _last);
	static QString getType(QString commandString, bool doSub = true);
	static ICommand* interpret(QString commandString, bool dry = false);
	static void initFormats();
};

#endif
