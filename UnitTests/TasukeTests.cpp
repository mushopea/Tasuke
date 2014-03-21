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
		app = new QApplication(argc, argv);
		std::thread ([=]() {
			app->exec();
		}).detach();
		Tasuke::instance();
		storage = nullptr;
	}

	TEST_MODULE_CLEANUP(ModuleCleanup) {
		app->quit();
		delete app;
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
		
		TEST_METHOD(TestInterpretBadCommand) {
			auto func = [this] {
				ICommand* command = Interpreter::interpret("bad command blah blah");
				command->run();
			};

			Assert::ExpectException<ExceptionBadCommand>(func);
		}

		TEST_METHOD(TestInterpretAddCommand) {
			Tasuke::instance().runCommand("add do homework");
			Assert::AreEqual(storage->totalTasks(), 1);

			Task task = storage->getTask(0);
			Assert::AreEqual(task.getDescription(), QString("do homework"));
			Assert::IsFalse(task.getBegin().isValid());
			Assert::IsFalse(task.getEnd().isValid());
			Assert::AreEqual(task.getTags().size(), 0);
		}

		TEST_METHOD(TestInterpretRemoveCommand) {
			Tasuke::instance().runCommand("add do homework");
			Tasuke::instance().runCommand("add buy eggs");
			Tasuke::instance().runCommand("add watch anime");
			Tasuke::instance().runCommand("remove 2");
			Assert::AreEqual(storage->totalTasks(), 2);
		}

		TEST_METHOD(TestInterpretEditCommand) {
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

		TEST_METHOD(TestInterpretUndoCommand) {
			for (int i=0; i<MAX_TASKS; i++) {
				Tasuke::instance().runCommand(QString("add task %1").arg(i));
			}
			Assert::AreEqual(storage->totalTasks(), MAX_TASKS);

			for (int i=0; i<MAX_TASKS; i++) {
				Tasuke::instance().runCommand("undo");
			}
			Assert::AreEqual(storage->totalTasks(), 0);
		}

		TEST_METHOD(TestInterpretRedoCommand) {
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
	};
}