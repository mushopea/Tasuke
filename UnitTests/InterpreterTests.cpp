//@author A0096836M

#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InterpreterTests {
	QApplication *app;
	StorageStub *storage;

	// Simulate running the main() function
	// Sets up the logging facility and the Qt event loop
	TEST_MODULE_INITIALIZE(ModuleInitialize) {
		int argc = 1;
		const char *argv[] = { TASUKE };
		FLAGS_logtostderr = true;
		google::InitGoogleLogging(argv[0]);
		app = new QApplication(argc, argv);
		Tasuke::setGuiMode(false);
		Tasuke::instance();
		storage = nullptr;
	}

	// Cleans up what we set up
	TEST_MODULE_CLEANUP(ModuleCleanup) {
		if (storage == nullptr) {
			delete storage;
		}

		app->quit();
		delete app;
	}

	TEST_CLASS(InterpreterTests) {

	public:

		// Create a new storage object before running any method
		TEST_METHOD_INITIALIZE(init) {
			storage = new StorageStub();
			Tasuke::instance().setStorage(storage);
		}
		
		// Cleans up after running any method
		TEST_METHOD_CLEANUP(deinit) {
			delete storage;
			storage = nullptr;
		}
		
		// Try interpretting an add command  (acceptable command parition)
		TEST_METHOD(InterpretAdd) {
			ICommand* command = Interpreter::interpret("add something @ 1pm - 3pm #tag");
			Assert::IsTrue(typeid(*command) == typeid(AddCommand));
			delete command;
		}

		// Try interpretting a remove command (acceptable command parition)
		TEST_METHOD(InterpretRemove) {
			Tasuke::instance().runCommand("add do homework");
			ICommand* command = Interpreter::interpret("remove 1");
			Assert::IsTrue(typeid(*command) == typeid(RemoveCommand) || typeid(*command) == typeid(CompositeCommand));
			delete command;
		}

		// Try interpretting a remove command below the lower bound partitionn
		TEST_METHOD(InterpretRemoveLowerBound) {
			Tasuke::instance().runCommand("add do homework");
			Tasuke::instance().runCommand("add buy eggs");
			Tasuke::instance().runCommand("add watch anime");
			Assert::ExpectException<ExceptionBadCommand>([] {
				ICommand* command = Interpreter::interpret("remove 0");
				command->run();
			});
			Tasuke::instance().runCommand("remove 0");
			Assert::AreEqual(storage->totalTasks(), 3);
		}

		// Try interpretting a remove command above the upper bound partition
		TEST_METHOD(InterpretRemovedUpperBound) {
			Tasuke::instance().runCommand("add do homework");
			Tasuke::instance().runCommand("add buy eggs");
			Tasuke::instance().runCommand("add watch anime");
			Assert::ExpectException<ExceptionBadCommand>([] {
				ICommand* command = Interpreter::interpret("remove 4");
				command->run();
			});
			Tasuke::instance().runCommand("remove 4");
			Assert::AreEqual(storage->totalTasks(), 3);
		}

		// Try interpretting an edit (acceptable command parition)
		TEST_METHOD(InterpretEdit) {
			Tasuke::instance().runCommand("add do homework");
			ICommand* command = Interpreter::interpret("edit 1 homework @ 2pm - 5pm #engineering");
			Assert::IsTrue(typeid(*command) == typeid(EditCommand));
			delete command;
		}

		// Try interpretting an edit below the lower bound partitionn
		TEST_METHOD(InterpretEditLowerBound) {
			Tasuke::instance().runCommand("add do homework");
			Tasuke::instance().runCommand("add buy eggs");
			Tasuke::instance().runCommand("add watch anime");
			Assert::ExpectException<ExceptionBadCommand>([] {
				ICommand* command = Interpreter::interpret("edit 0 fly kite");
				command->run();
			});
			Tasuke::instance().runCommand("edit 0 fly kite");
		}

		// Try interpretting an edit above the upper bound partitionn
		TEST_METHOD(InterpretEditUpperBound) {
			Tasuke::instance().runCommand("add do homework");
			Tasuke::instance().runCommand("add buy eggs");
			Tasuke::instance().runCommand("add watch anime");
			Assert::ExpectException<ExceptionBadCommand>([] {
				ICommand* command = Interpreter::interpret("edit 4 fly kite");
				command->run();
			});
			Tasuke::instance().runCommand("edit 4 fly kite");
		}

		// Try interpretting a done (acceptable command partition)
		TEST_METHOD(InterpretDone) {
			Tasuke::instance().runCommand("add do homework");
			ICommand* command = Interpreter::interpret("done 1");
			Assert::IsTrue(typeid(*command) == typeid(DoneCommand) || typeid(*command) == typeid(CompositeCommand));
			delete command;
		}

		// Try interpretting a done below the lower bound partitionn
		TEST_METHOD(InterpretDoneLowerBound) {
			Tasuke::instance().runCommand("add do homework");
			Tasuke::instance().runCommand("add buy eggs");
			Tasuke::instance().runCommand("add watch anime");
			Assert::ExpectException<ExceptionBadCommand>([] {
				ICommand* command = Interpreter::interpret("done 0");
				command->run();
			});
			Tasuke::instance().runCommand("done 0");
		}

		// Try interpretting a done above the upper bound partitionn
		TEST_METHOD(InterpretDoneUpperBound) {
			Tasuke::instance().runCommand("add do homework");
			Tasuke::instance().runCommand("add buy eggs");
			Tasuke::instance().runCommand("add watch anime");
			Assert::ExpectException<ExceptionBadCommand>([] {
				ICommand* command = Interpreter::interpret("done 4");
				command->run();
			});
			Tasuke::instance().runCommand("done 4");
		}

		// Try interpretting an undone (acceptable command partition)
		TEST_METHOD(InterpretUndone) {
			Tasuke::instance().runCommand("add do homework");
			ICommand* command = Interpreter::interpret("undone 1");
			Assert::IsTrue(typeid(*command) == typeid(DoneCommand) || typeid(*command) == typeid(CompositeCommand));
			delete command;
		}

		// Try interpretting an undone below the lower bound partition
		TEST_METHOD(InterpretUndoneLowerBound) {
			Tasuke::instance().runCommand("add do homework");
			Tasuke::instance().runCommand("add buy eggs");
			Tasuke::instance().runCommand("add watch anime");
			Assert::ExpectException<ExceptionBadCommand>([] {
				ICommand* command = Interpreter::interpret("undone 0");
				command->run();
			});
			Tasuke::instance().runCommand("undone 0");
		}

		// Try interpretting an undone above the upper bound partitionn
		TEST_METHOD(InterpretUndoneUpperBound) {
			Tasuke::instance().runCommand("add do homework");
			Tasuke::instance().runCommand("add buy eggs");
			Tasuke::instance().runCommand("add watch anime");
			Assert::ExpectException<ExceptionBadCommand>([] {
				ICommand* command = Interpreter::interpret("undone 4");
				command->run();
			});
			Tasuke::instance().runCommand("undone 4");
		}

		// Try interpretting all commands with nullptr return
		TEST_METHOD(InterpretNullReturn) {
			ICommand* command = Interpreter::interpret("show");
			Assert::IsTrue(command == nullptr);
			command = Interpreter::interpret("hide");
			Assert::IsTrue(command == nullptr);
			command = Interpreter::interpret("about");
			Assert::IsTrue(command == nullptr);
			command = Interpreter::interpret("help");
			Assert::IsTrue(command == nullptr);
		}
	};
}