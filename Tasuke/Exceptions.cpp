#include <QString>
#include "Constants.h"
#include "Exceptions.h"

//@author A0096836M

const char* EXCEPTION_NULL_PTR = "attempt to dereference null pointer";
const char* EXCEPTION_NOT_IMPLEMENTED = "not implemented";
const char* EXCEPTION_NO_MORE_TASKS = "no more tasks in the list";
const char* EXCEPTION_ICONSET_OUT_OF_RANGE = "out of range icon set was stored and attempted access in settings.";
const char* EXCEPTION_THEME_OUT_OF_RANGE = "out of range theme was stored and attempted access in settings.";


// This method returns a user readable error for the ExceptionNullPtr
// exception
const char* ExceptionNullPtr::what() const throw() {
	return EXCEPTION_NULL_PTR;
}

ExceptionBadCommand::ExceptionBadCommand(QString _message, QString _part) : message(_message), part(_part) {

}

// This method returns a user readable error for the ExceptionBadCommand
// exception
const char* ExceptionBadCommand::what() const throw() {
	return qstrdup(qPrintable(message));
}

QString ExceptionBadCommand::where() const {
	return part;
}

const char* ExceptionNotImplemented::what() const throw() {
	return EXCEPTION_NOT_IMPLEMENTED;
}

const char* ExceptionNoMoreTasks::what() const throw() {
	return EXCEPTION_NO_MORE_TASKS;
}

const char* ExceptionIconsetOutOfRange::what() const throw() {
	return EXCEPTION_ICONSET_OUT_OF_RANGE;
}

const char* ExceptionThemeOutOfRange::what() const throw() {
	return EXCEPTION_THEME_OUT_OF_RANGE;
}