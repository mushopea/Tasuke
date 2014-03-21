#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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
			Assert::AreEqual(task.getDescription().toStdString(), std::string("do homework"));
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

	};
}