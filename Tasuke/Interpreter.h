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
	
	static QHash<QString, QString> decompose(QString text);
	static QString removeBefore(QString text, QString before);
	static QString getType(QString commandString);
	static int parseId(QString idString);
	static TIME_PERIOD parseTimePeriod(QString timePeriod);
	static QDateTime parseDate(QString dateString);

	static AddCommand* createAddCommand(QString commandString);
	static RemoveCommand* createRemoveCommand(QString commandString);
	static EditCommand* createEditCommand(QString commandString);
	static void doShow();
	static void doAbout();
	static void doHide();
	static void doClear();
	static void doUndo();
	static void doRedo();
	static void doExit();

public:
	static ICommand* interpret(QString commandString);
};

#endif
