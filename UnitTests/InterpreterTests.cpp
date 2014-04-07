#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InterpreterTests {
	QApplication *app;
	StorageStub *storage;

	TEST_MODULE_INITIALIZE(ModuleInitialize) {
		int argc = 1;
		char *argv[] = { "Tasuke.exe" };
		FLAGS_logtostderr = true;
		google::InitGoogleLogging(argv[0]);
		app = new QApplication(argc, argv);
		Tasuke::setGuiMode(false);
		Tasuke::instance();
		storage = nullptr;
	}

	TEST_MODULE_CLEANUP(ModuleCleanup) {
		if (storage == nullptr) {
			delete storage;
		}

		app->quit();
		delete app;
	}

	TEST_CLASS(InterpreterTests) {

	public:

		TEST_METHOD_INITIALIZE(init) {
			storage = new StorageStub();
			Tasuke::instance().setStorage(storage);
		}

		TEST_METHOD_CLEANUP(deinit) {
			delete storage;
			storage = nullptr;
		}
		
		TEST_METHOD(InterpretAdd) {
			ICommand* command = Interpreter::interpret("add something @ 1pm - 3pm #tag");
			Assert::IsTrue(typeid(*command) == typeid(AddCommand));
			delete command;
		}

		TEST_METHOD(InterpretRemove) {
			Tasuke::instance().runCommand("add do homework");
			ICommand* command = Interpreter::interpret("remove 1");
			Assert::IsTrue(typeid(*command) == typeid(RemoveCommand) || typeid(*command) == typeid(CompositeCommand));
			delete command;
		}

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

		TEST_METHOD(InterpretEdit) {
			Tasuke::instance().runCommand("add do homework");
			ICommand* command = Interpreter::interpret("edit 1 homework @ 2pm - 5pm #engineering");
			Assert::IsTrue(typeid(*command) == typeid(EditCommand));
			delete command;
		}

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

		TEST_METHOD(InterpretDone) {
			Tasuke::instance().runCommand("add do homework");
			ICommand* command = Interpreter::interpret("done 1");
			Assert::IsTrue(typeid(*command) == typeid(DoneCommand) || typeid(*command) == typeid(CompositeCommand));
			delete command;
		}

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

		TEST_METHOD(InterpretUndone) {
			Tasuke::instance().runCommand("add do homework");
			ICommand* command = Interpreter::interpret("undone 1");
			Assert::IsTrue(typeid(*command) == typeid(DoneCommand) || typeid(*command) == typeid(CompositeCommand));
			delete command;
		}

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