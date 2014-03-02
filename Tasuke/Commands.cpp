#include "Tasuke.h"
#include "Constants.h"
#include "Commands.h"

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
	Tasuke::instance().showMessage(QString("Undo add \"%1\"").arg(task.getDescription()));
}
