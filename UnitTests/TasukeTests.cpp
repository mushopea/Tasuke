#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(TasukeTests)
	{
	
	public:
		
		TEST_METHOD(TestInterpretBadCommand)
		{
			auto func = [this] {
				Interpreter::interpret("bad command");
			};

			Assert::ExpectException<ExceptionBadCommand>(func);
		}

		TEST_METHOD(TestInterpretAddCommand)
		{
			ICommand* command = Interpreter::interpret("add task");
			dynamic_cast<AddCommand*>(command);
		}

	};
}