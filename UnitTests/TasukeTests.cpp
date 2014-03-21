#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{	
	TEST_CLASS(TasukeTests) {

	private:
		QApplication *app;

		void init() {
			int argc = 1;
			char *argv[] = { "Tasuke.exe" };
			app = new QApplication(argc, argv);
			std::thread ([=]() {
				app->exec();
			}).detach();
			Tasuke::instance();
		}

		void cleanup() {
			app->quit();
			delete app;
		}
	
	public:
		
		TEST_METHOD(TestInterpretBadCommand) {
			init();

			auto func = [this] {
				Interpreter::interpret("bad command");
			};

			Assert::ExpectException<ExceptionBadCommand>(func);

			cleanup();
		}

		TEST_METHOD(TestInterpretAddCommand) {
			init();

			ICommand* command = Interpreter::interpret("add task");
			dynamic_cast<AddCommand*>(command);

			cleanup();
		}

		/*TEST_METHOD(TestInterpretRemoveCommand) {
			init();

			ICommand* command = Interpreter::interpret("remove 1");
			dynamic_cast<RemoveCommand*>(command);

			cleanup();
		}*/

	};
}