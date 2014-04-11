#include <cassert>
#include "Tasuke.h"
#include "Constants.h"
#include "Commands.h"
#include "Interpreter.h"

//@author A0096836M

ICommand::ICommand() {
	hasRun = false;
}

ICommand::~ICommand() {

}

void ICommand::run() {
	assert(hasRun == false);
	hasRun = true;
}

void ICommand::undo() {
	assert(hasRun == true);
	hasRun = false;
}

AddCommand::AddCommand(Task& _task) : task(_task) {

}

AddCommand::~AddCommand() {

}
	
void AddCommand::run() {
	task = Tasuke::instance().getStorage().addTask(task);
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
	Tasuke::instance().highlightTask(task.getId());
	Interpreter::setLast(task.getId()+1);
}
void AddCommand::undo() {
	Tasuke::instance().getStorage().removeTask(task.getId());
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
}

RemoveCommand::RemoveCommand(int _id) {
	id = _id;
	task = Tasuke::instance().getStorage().getTask(id);
}

RemoveCommand::~RemoveCommand() {

}
	
void RemoveCommand::run() {
	Tasuke::instance().getStorage().removeTask(id);
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
}
void RemoveCommand::undo() {
	Tasuke::instance().getStorage().addTask(task);
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
}

EditCommand::EditCommand(int _id, Task& _task) : id(_id), task(_task) {
	
}
EditCommand::~EditCommand() {

}

void EditCommand::run() {
	old = Tasuke::instance().getStorage().getTask(id);
	task = Tasuke::instance().getStorage().editTask(id, task);
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
	Tasuke::instance().highlightTask(task.getId());
	Interpreter::setLast(task.getId()+1);
}
void EditCommand::undo() {
	Tasuke::instance().getStorage().editTask(task.getId(), old);
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
	Tasuke::instance().highlightTask(old.getId());
	Interpreter::setLast(task.getId()+1);
}

ClearCommand::ClearCommand() {

}
ClearCommand::~ClearCommand() {

}

void ClearCommand::run() {
	old = QList<Task>(Tasuke::instance().getStorage().getTasks());
	Tasuke::instance().getStorage().clearAllTasks();
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
}
void ClearCommand::undo() {
	for (int i=0; i<old.size(); i++) {
		Tasuke::instance().getStorage().addTask(old[i]);
	}
	Tasuke::instance().updateTaskWindow(Tasuke::instance().getStorage().getTasks());
}

DoneCommand::DoneCommand(int _id, bool _done) : id(_id), done(_done) {

}
DoneCommand::~DoneCommand() {

}
	
void DoneCommand::run() {
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
void DoneCommand::undo() {
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

CompositeCommand::CompositeCommand(QList< QSharedPointer<ICommand> > _commands) : commands(_commands) {
	
}

CompositeCommand::~CompositeCommand() {
	
}

void CompositeCommand::run() {
	foreach(QSharedPointer<ICommand> command, commands) {
		command->run();
	}
}

void CompositeCommand::undo() {
	// must be in reverse order
	for(int i=commands.size()-1; i>=0; i--) {
		commands[i]->undo();
	}
}