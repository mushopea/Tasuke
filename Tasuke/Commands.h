#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>

// This is an interface for all user commands. The intended method to intialize
// a ICommand instance is through the CommandFactory.
class ICommand {
public:
	virtual ~ICommand() {};
	
	virtual void run() = 0;
	virtual void undo() = 0;
};

// TODO: define commands here

#endif
