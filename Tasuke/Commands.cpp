//@author A0096836M

#include <cassert>
#include "Tasuke.h"
#include "Constants.h"
#include "Commands.h"
#include "Interpreter.h"

// Constructor for ICommand
ICommand::ICommand() {
	hasRun = false;
}

// Destructor for ICommand
ICommand::~ICommand() {

}

// This ensures a command has only been run once unless undone
void ICommand::run() {
	assert(hasRun == false);
	hasRun = true;
}

// This ensures a command can only be undone after being run
void ICommand::undo() {
	assert(hasRun == true);
	hasRun = false;
}

// Constructor for AddCommand. Takes in a task object to add
AddCommand::AddCommand(Task& _task) : task(_task) {

}

// Descrutor for AddCommand
AddCommand::~AddCommand() {

}

// Adds the task given in the constructor
void AddCommand::run() {
	ICommand::run();

	task = Tasuke::instance().getStorage().addTask(task);
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
	Tasuke::instance().highlightTask(task.getId());
	Interpreter::setLast(task.getId()+1);
}

// Undos adding the task.
void AddCommand::undo() {
	ICommand::undo();

	Tasuke::instance().getStorage().removeTask(task.getId());
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
}

// Constructor for RemoveCommand. Takes in an id of a task to remove
RemoveCommand::RemoveCommand(int _id) {
	id = _id;
	task = Tasuke::instance().getStorage().getTask(id);
}

// Destructor for RemoveCommand..
RemoveCommand::~RemoveCommand() {

}

// Removes the task with the id given in the constructor
void RemoveCommand::run() {
	ICommand::run();

	Tasuke::instance().getStorage().removeTask(id);
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
}

// Undoes removing the task
void RemoveCommand::undo() {
	ICommand::undo();

	Tasuke::instance().getStorage().addTask(task);
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
}

// Constructor for EditCommand. Takes in an id of a task to replace with the 
// task given.
EditCommand::EditCommand(int _id, Task& _task) : id(_id), task(_task) {
	
}

// Destructor for EditCommand
EditCommand::~EditCommand() {

}

// Replaces the task with the id with the task given in the constructor
void EditCommand::run() {
	ICommand::run();

	old = Tasuke::instance().getStorage().getTask(id);
	task = Tasuke::instance().getStorage().editTask(id, task);
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
	Tasuke::instance().highlightTask(task.getId());
	Interpreter::setLast(task.getId()+1);
}

// Undos the edit
void EditCommand::undo() {
	ICommand::undo();

	Tasuke::instance().getStorage().editTask(task.getId(), old);
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
	Tasuke::instance().highlightTask(old.getId());
	Interpreter::setLast(task.getId()+1);
}

// Constructor for ClearCommand
ClearCommand::ClearCommand() {

}

// Desctructor for ClearCommand
ClearCommand::~ClearCommand() {

}

// Clears all tasks in Storage
void ClearCommand::run() {
	ICommand::run();

	old = QList<Task>(Tasuke::instance().getStorage().getTasks());
	Tasuke::instance().getStorage().clearAllTasks();
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
}

// Undos clearing all tasks
void ClearCommand::undo() {
	ICommand::undo();

	for (int i=0; i<old.size(); i++) {
		Tasuke::instance().getStorage().addTask(old[i]);
	}
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
}

// Constructor for DoneCommand. Takes in an id of the task to mark and a bool
// to mark as done or undone. Defaults to done
DoneCommand::DoneCommand(int _id, bool _done) : id(_id), done(_done) {

}

// Desctuctor for DoneCommand.
DoneCommand::~DoneCommand() {

}
	
// Marks the task with the id given in the constructor as done/undone
void DoneCommand::run() {
	ICommand::run();

	Task task = Tasuke::instance().getStorage().getTask(id);
	task.setDone(done);
	task = Tasuke::instance().getStorage().editTask(id, task);
	id = task.getId();
	QString doneUndone = done ? "done" : "undone";
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
	if (!done) {
		Tasuke::instance().highlightTask(task.getId());
	}
}

// Undos marking the task as done/undone
void DoneCommand::undo() {
	ICommand::undo();

	Task task = Tasuke::instance().getStorage().getTask(id);
	task.setDone(!done);
	task = Tasuke::instance().getStorage().editTask(id, task);
	id = task.getId();
	QString doneUndone = done ? "done" : "undone";
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
	if (!done) {
		Tasuke::instance().highlightTask(task.getId());
	}
}

// Constructor for CompositeCommand. Takes in a list of ICommands to run as a
// single ICommand. The commands will be run in the order given.
CompositeCommand::CompositeCommand(QList< QSharedPointer<ICommand> > _commands) : commands(_commands) {
	
}

// Destructor for CompositeCommand.
CompositeCommand::~CompositeCommand() {
	
}

// Runs all the ICommands in the order given in the constructor.
void CompositeCommand::run() {
	ICommand::run();

	foreach(QSharedPointer<ICommand> command, commands) {
		command->run();
	}
}

// Undos all the ICommands in the reverse order given in the constructor.
void CompositeCommand::undo() {
	ICommand::undo();

	// must be in reverse order
	for(int i=commands.size()-1; i>=0; i--) {
		commands[i]->undo();
	}
}