#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <memory>
#include <string>

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

	static QStringList timeFormats;
	static QStringList dateFormatsWithoutYear;
	static QStringList dateFormats;
	static QStringList dateTimeFormats;
	static bool formatsAlreadyInit;
	
	static QString substitute(QString text);
	static QHash<QString, QString> decompose(QString text);
	static QString removeBefore(QString text, QString before);
	static int parseId(QString idString);
	static TIME_PERIOD parseTimePeriod(QString timePeriod);
	static QDateTime parseDate(QString dateString, bool isEnd = true);
	static void initFormats();
	static void generateTimeFormats();
	static void generateDateFormatsWithoutYear();
	static void generateDateFormats();
	static void generateDateTimeFormats();

	static AddCommand* createAddCommand(QString commandString);
	static RemoveCommand* createRemoveCommand(QString commandString);
	static EditCommand* createEditCommand(QString commandString);
	static ClearCommand* createClearCommand(QString commandString);
	static DoneCommand* createDoneCommand(QString commandString);
	static DoneCommand* createUndoneCommand(QString commandString);

	static void doShow();
	static void doAbout();
	static void doHide();
	static void doUndo();
	static void doRedo();
	static void doHelp();
	static void doSettings();
	static void doExit();

public:	
	static QString getType(QString commandString);
	static ICommand* interpret(QString commandString);
};

#endif
