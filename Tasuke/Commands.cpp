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
}
void AddCommand::undo() {
	Tasuke::instance().getStorage().popTask();
}
