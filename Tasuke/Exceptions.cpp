#include "Constants.h"
#include "Exceptions.h"

const char* EXCEPTION_NULL_PTR = "attempt to dereference null pointer";
const char* EXCEPTION_BAD_COMMAND = "bad command given";
const char* EXCEPTION_NOT_IMPLEMENTED = "not implemented";

// This method returns a user readable error for the ExceptionNullPtr
// exception
const char* ExceptionNullPtr::what() const throw()
{
	return EXCEPTION_NULL_PTR;
}

// This method returns a user readable error for the ExceptionBadCommand
// exception
const char* ExceptionBadCommand::what() const throw()
{
	return EXCEPTION_BAD_COMMAND;
}

const char* ExceptionNotImplemented::what() const throw()
{
	return EXCEPTION_NOT_IMPLEMENTED;
}