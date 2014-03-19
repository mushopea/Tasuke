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
	static QDateTime parseDate(QString dateString);
public:
	static ICommand* interpret(QString command);
};

#endif
