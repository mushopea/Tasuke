#include "Constants.h"
#include "Exceptions.h"

//@author A0096836M

const char* EXCEPTION_NULL_PTR = "attempt to dereference null pointer";
const char* EXCEPTION_BAD_COMMAND = "bad command given";
const char* EXCEPTION_NOT_IMPLEMENTED = "not implemented";
const char* EXCEPTION_TOO_MANY_TAGS = "exceeded tag limit";

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

const char* ExceptionTooManyTags::what() const throw()
{
	return EXCEPTION_TOO_MANY_TAGS;
}