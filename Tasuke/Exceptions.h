#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

//@author A0096836M

// This exception is thrown only during programming errors. If this happens,
// then it indicates a bug in the code
class ExceptionNullPtr : public std::exception
{
	virtual const char *what() const throw();
};

// This exception is thrown when an invalid command is input by the user
class ExceptionBadCommand : public std::exception
{
	virtual const char *what() const throw();
};

class ExceptionNotImplemented : public std::exception
{
	virtual const char *what() const throw();
};

// This exception is thrown when the user tries to add too many tags.
class ExceptionTooManyTags : public std::exception
{
	virtual const char *what() const throw();
};

#endif
