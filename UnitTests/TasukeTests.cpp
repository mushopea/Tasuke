#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

const int MAX_TASKS = 20;

namespace Microsoft { 
    namespace VisualStudio { 
        namespace CppUnitTestFramework {
			template<> static std::wstring ToString<QString>(const QString& string) {
				RETURN_WIDE_STRING(string.toStdString().data());
			}

			template<> static std::wstring ToString<QDate>(const QDate& date) {
				RETURN_WIDE_STRING(date.toString().toStdString().data());
			}

			template<> static std::wstring ToString<QTime>(const QTime& date) {
				RETURN_WIDE_STRING(date.toString().toStdString().data());
			}

			template<> static std::wstring ToString<QDateTime>(const QDateTime& date) {
				RETURN_WIDE_STRING(date.toString().toStdString().data());
			}
		}
	}
}

namespace UnitTest {
	QApplication *app;
	StorageStub *storage;

	TEST_MODULE_INITIALIZE(ModuleInitialize) {
		int argc = 1;
		char *argv[] = { "Tasuke.exe" };
		FLAGS_logtostderr = true;
		google::InitGoogleLogging(argv[0]);
		Tasuke::setGuiMode(false);
		Tasuke::instance();
		storage = nullptr;
	}

	TEST_MODULE_CLEANUP(ModuleCleanup) {
		if (storage == nullptr) {
			delete storage;
		}
	}

	TEST_CLASS(TasukeTests) {

	public:

		TEST_METHOD_INITIALIZE(init) {
			storage = new StorageStub();
			Tasuke::instance().setStorage(storage);
		}

		TEST_METHOD_CLEANUP(deinit) {
			delete storage;
			storage = nullptr;
		}
		
		TEST_METHOD(BadInput) {
			Assert::ExpectException<ExceptionBadCommand>([] {
				ICommand* command = Interpreter::interpret("bad command blah blah");
				command->run();
			});
			Tasuke::instance().runCommand("bad command blah blah");
			Assert::AreEqual(storage->totalTasks(), 0);
		}

		TEST_METHOD(AddingTasks) {
			Tasuke::instance().runCommand("add do homework");
			Assert::AreEqual(storage->totalTasks(), 1);

			Task task = storage->getTask(0);
			Assert::AreEqual(task.getDescription(), QString("do homework"));
			Assert::IsFalse(task.getBegin().isValid());
			Assert::IsFalse(task.getEnd().isValid());
			Assert::AreEqual(task.getTags().size(), 0);
		}

		TEST_METHOD(RemovingTasks) {
			Tasuke::instance().runCommand("add do homework");
			Tasuke::instance().runCommand("add buy eggs");
			Tasuke::instance().runCommand("add watch anime");
			Tasuke::instance().runCommand("remove 2");
			Assert::AreEqual(storage->totalTasks(), 2);
		}

		TEST_METHOD(EditingTasks) {
			Tasuke::instance().runCommand("add do homework");
			Tasuke::instance().runCommand("add buy eggs");
			Tasuke::instance().runCommand("add watch anime");
			Tasuke::instance().runCommand("edit 2 buy milk @ 2pm - 3pm #shopping");
			Assert::AreEqual(storage->totalTasks(), 3);

			Task task = storage->getTask(1);
			Assert::AreEqual(task.getDescription(), QString("buy milk"));
			Assert::IsTrue(task.getBegin().isValid());
			Assert::AreEqual(task.getBegin().date(), QDate::currentDate());
			Assert::AreEqual(task.getBegin().time(), QTime(14,0));
			Assert::IsTrue(task.getEnd().isValid());
			Assert::AreEqual(task.getEnd().date(), QDate::currentDate());
			Assert::AreEqual(task.getEnd().time(), QTime(15,0));
			Assert::AreEqual(task.getTags().size(), 1);
			Assert::AreEqual(task.getTags()[0], QString("shopping"));
		}

		TEST_METHOD(UndoingTasks) {
			for (int i=0; i<MAX_TASKS; i++) {
				Tasuke::instance().runCommand(QString("add task %1").arg(i));
			}
			Assert::AreEqual(storage->totalTasks(), MAX_TASKS);

			for (int i=0; i<MAX_TASKS; i++) {
				Tasuke::instance().runCommand("undo");
			}
			Assert::AreEqual(storage->totalTasks(), 0);
		}

		TEST_METHOD(RedoingTasks) {
			for (int i=0; i<MAX_TASKS; i++) {
				Tasuke::instance().runCommand(QString("add task %1").arg(i));
			}
			Assert::AreEqual(storage->totalTasks(), MAX_TASKS);

			for (int i=0; i<MAX_TASKS; i++) {
				Tasuke::instance().runCommand("undo");
			}
			Assert::AreEqual(storage->totalTasks(), 0);

			for (int i=0; i<MAX_TASKS; i++) {
				Tasuke::instance().runCommand("redo");
			}
			Assert::AreEqual(storage->totalTasks(), MAX_TASKS);
		}

		TEST_METHOD(InterpretAdd) {
			ICommand* command = Interpreter::interpret("add something @ 1pm - 3pm #tag");
			Assert::IsTrue(typeid(*command) == typeid(AddCommand));
			delete command;
		}

		TEST_METHOD(InterpretRemove) {
			Tasuke::instance().runCommand("add do homework");
			ICommand* command = Interpreter::interpret("remove 1");
			Assert::IsTrue(typeid(*command) == typeid(RemoveCommand));
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
			Assert::IsTrue(typeid(*command) == typeid(DoneCommand));
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
			Assert::IsTrue(typeid(*command) == typeid(DoneCommand));
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