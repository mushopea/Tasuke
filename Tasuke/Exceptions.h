#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <QString>

//@author A0096836M

// This exception is thrown only during programming errors. If this happens,
// then it indicates a bug in the code
class ExceptionNullPtr : public std::exception {
	virtual const char *what() const throw();
};

// This exception is thrown when an invalid command is input by the user
class ExceptionBadCommand : public std::exception {
private:
	QString message;
	QString part;
public:
	ExceptionBadCommand(QString _message, QString _part = "");
	virtual const char *what() const throw();
	QString where() const;
};

class ExceptionNotImplemented : public std::exception {
	virtual const char *what() const throw();
};

// This exception is thrown when the user tries to add too many tags.
class ExceptionTooManyTags : public std::exception {
	virtual const char *what() const throw();
};

// This exception is thrown when there are no upcoming tasks.
class ExceptionNoMoreTasks : public std::exception {
	virtual const char *what() const throw();
};

// This exception is thrown when iconset loaded from settings is out of range
class ExceptionIconsetOutOfRange : public std::exception {
	virtual const char *what() const throw();
};

// This exception is thrown when theme loaded from settings is out of range
class ExceptionThemeOutOfRange : public std::exception {
	virtual const char *what() const throw();
};

#endif
