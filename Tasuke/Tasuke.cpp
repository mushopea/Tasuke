#include <string>
#include <iostream>
#include <QMessageBox>
#include "Constants.h"
#include "Exceptions.h"
#include "CommandFactory.h"
#include "Commands.h"
#include "Tasuke.h"

// Constructor for the Tasuke singleton.
Tasuke::Tasuke() {
	storage = new Storage();
	storage->loadFile();
}

// Destructor for the Tasuke singleton.
Tasuke::~Tasuke() {
	if (storage != nullptr) {
		delete storage;
	}
}

// Static method that returns the sole instance of Tasuke.
Tasuke& Tasuke::instance() {
	static Tasuke *instance = new Tasuke();
	return *instance;
}

void Tasuke::setStorage(Storage* _storage) {
	delete storage;
	storage = _storage;
}

// This function exposes the Storage instance for editing.
Storage& Tasuke::getStorage() {
	if (storage == nullptr) {
		throw ExceptionNullPtr();
	}
	return *storage;
}

void Tasuke::showInputWindow() {
	inputWindow.showAndCenter();
}

// This function runs a command in a string
void Tasuke::runCommand(std::string commandString) {
	try {
		std::shared_ptr<ICommand> command = CommandFactory::interpret(commandString);
		command->run();
		commandUndoHistory.push_back(command);
		commandRedoHistory.clear();
	} catch (ExceptionBadCommand exception) {
		QMessageBox::information(&inputWindow, "Tasuke", "Unknown command");
	}
}

void Tasuke::undoCommand() {
	if (commandUndoHistory.size() == 0) {
		// Nothing to undo
		return;
	}

	std::shared_ptr<ICommand> command = commandUndoHistory.back();
	commandUndoHistory.pop_back();
	command->undo();
	commandRedoHistory.push_back(command);
}

void Tasuke::redoCommand() {
	if (commandRedoHistory.size() == 0) {
		// Nothing to redo
		return;
	}

	std::shared_ptr<ICommand> command = commandRedoHistory.back();
	commandRedoHistory.pop_back();
	command->run();
	commandUndoHistory.push_back(command);
}