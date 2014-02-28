#include <iostream>
#include "Constants.h"
#include "Exceptions.h"
#include "Storage.h"

// Constructor for Storage.
Storage::Storage() {
	
}

// This function loads the contents of the text file and serialize it into
// the memory. If there is no such file, this function does nothing.
void Storage::loadFile() {
	// TODO: implement storage
	throw ExceptionNotImplemented();
}

// This function deserialize the data from memory and writes it to the text
// file. If the file cannot be written, an ExceptionNotOpen is thrown.
void Storage::saveFile() {
	// TODO: implement storage
	throw ExceptionNotImplemented();
}

void addTask(Task& task) {
	// TODO: implement storage
	throw ExceptionNotImplemented();
}

Task& getTask(int i) {
	// TODO: implement storage
	throw ExceptionNotImplemented();
}

void removeTask(int i) {
	// TODO: implement storage
	throw ExceptionNotImplemented();
}