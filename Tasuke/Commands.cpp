#include <cassert>
#include "Tasuke.h"
#include "Constants.h"
#include "Commands.h"

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

AddCommand::AddCommand(Task& _task) {
	task = _task;
}

AddCommand::~AddCommand() {

}
	
void AddCommand::run() {
	Tasuke::instance().getStorage().addTask(task);
	Tasuke::instance().showMessage(QString("Added \"%1\"").arg(task.getDescription()));
}
void AddCommand::undo() {
	Tasuke::instance().getStorage().popTask();
	Tasuke::instance().showMessage(QString("Undone add \"%1\"").arg(task.getDescription()));
}

RemoveCommand::RemoveCommand(int _id) {
	id = _id;
	task = Tasuke::instance().getStorage().getTask(id);
}

RemoveCommand::~RemoveCommand() {

}
	
void RemoveCommand::run() {
	Tasuke::instance().getStorage().removeTask(id);
	Tasuke::instance().showMessage(QString("Removed \"%1\"").arg(task.getDescription()));
}
void RemoveCommand::undo() {
	Tasuke::instance().getStorage().addTask(task, id);
	Tasuke::instance().showMessage(QString("Undone remove \"%1\"").arg(task.getDescription()));
}

EditCommand::EditCommand(int _id, Task& _task) {
	id = _id;
	task = _task;
}
EditCommand::~EditCommand() {

}

void EditCommand::run() {
	old = Tasuke::instance().getStorage().getTask(id);;
	Tasuke::instance().getStorage().removeTask(id);
	Tasuke::instance().getStorage().addTask(task, id);
	Tasuke::instance().showMessage(QString("Edited \"%1\"").arg(task.getDescription()));
}
void EditCommand::undo() {
	Tasuke::instance().getStorage().removeTask(id);
	Tasuke::instance().getStorage().addTask(old, id);
	Tasuke::instance().showMessage(QString("Undone edit \"%1\"").arg(task.getDescription()));
}