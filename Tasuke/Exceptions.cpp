//@author A0096836M

#include <QString>
#include "Constants.h"
#include "Exceptions.h"

// This method returns a user readable error for the ExceptionNullPtr
// exception
const char* ExceptionNullPtr::what() const throw() {
	return EXCEPTION_NULL_PTR;
}

// Constructor for ExceptionBadCommand. It takes in the error message as well as
// the location in the command the error is referring to. If it is the whole
// command the location should be empty string. The location defaults to empty
// string
ExceptionBadCommand::ExceptionBadCommand(QString _message, QString _part) : message(_message), part(_part) {

}

// This method returns a user readable error for the ExceptionBadCommand
// exception
const char* ExceptionBadCommand::what() const throw() {
	return qstrdup(qPrintable(message));
}

// This methods returns the location 
QString ExceptionBadCommand::where() const {
	return part;
}

// This method returns a user readable error for the ExceptionNotImplemented
// exception
const char* ExceptionNotImplemented::what() const throw() {
	return EXCEPTION_NOT_IMPLEMENTED;
}

// This method returns a user readable error for the ExceptionNoMoreTasks
// exception
const char* ExceptionNoMoreTasks::what() const throw() {
	return EXCEPTION_NO_MORE_TASKS;
}

// This method returns a user readable error for the ExceptionIconsetOutOfRange
// exception
const char* ExceptionIconsetOutOfRange::what() const throw() {
	return EXCEPTION_ICONSET_OUT_OF_RANGE;
}

// This method returns a user readable error for the ExceptionThemeOutOfRange
// exception
const char* ExceptionThemeOutOfRange::what() const throw() {
	return EXCEPTION_THEME_OUT_OF_RANGE;
}