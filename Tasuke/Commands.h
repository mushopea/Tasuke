#ifndef COMMANDS_H
#define COMMANDS_H

#include "Task.h"

// This is an interface for all user commands. The intended method to intialize
// a ICommand instance is through the CommandFactory.
class ICommand {
public:
	virtual ~ICommand() {};
	
	virtual void run() = 0;
	virtual void undo() = 0;
};

class AddCommand : public ICommand {
private:
	Task task;
public:
	AddCommand(Task& _task);
	~AddCommand();
	
	void run();
	void undo();
};

class RemoveCommand : public ICommand {
private:
	int id;
	Task task;
public:
	RemoveCommand(int _id);
	~RemoveCommand();
	
	void run();
	void undo();
};

#endif
