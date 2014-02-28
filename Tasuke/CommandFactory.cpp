#include <memory>
#include "Commands.h"
#include "Constants.h"
#include "Exceptions.h"
#include "CommandFactory.h"

// This static helper function returns an instance of a ICommand that represents
// the user's command. The caller must clean up using delete.
std::shared_ptr<ICommand> CommandFactory::interpret(const std::string& command) {
	// TODO: implement interpreter
	throw ExceptionBadCommand();
}