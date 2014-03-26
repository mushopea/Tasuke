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

namespace StorageTests {
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

	TEST_CLASS(StorageTests) {

	public:

		TEST_METHOD_INITIALIZE(init) {
			storage = new StorageStub();
			Tasuke::instance().setStorage(storage);
		}

		TEST_METHOD_CLEANUP(deinit) {
			delete storage;
			storage = nullptr;
		}
		
		// Should throw an exception because exceed maximum tag count.
		TEST_METHOD(TaskExceedMaximumTagCount) {
			Assert::ExpectException<ExceptionTooManyTags>([] {
				Task task;
				for (int i=0; i<MAXIMUM_TAGS+1; i++) {
					task.addTag("tag" + i);
				}
			});
		}

		// Should not throw an exception because within maximum tag count.
		TEST_METHOD(TaskWithinMaximumTagCount) {
			Task task;
			for (int i=0; i<MAXIMUM_TAGS; i++) {
				task.addTag("tag" + i);
			}
		}

		// Task should NOT be overdue (due date is year 2100, never overdue).
		TEST_METHOD(TaskIsNotOverdue) {
			Task task;
			task.setEnd(QDateTime(QDate(2100, 12, 31),QTime(23, 59, 59)));
			Assert::IsFalse(task.isOverdue());
		}

		// Task should be overdue (due date is year 2010, always overdue).
		TEST_METHOD(TaskIsOverdue) {
			Task task;
			task.setEnd(QDateTime(QDate(2010, 1, 1), QTime(1, 1, 1)));
			Assert::IsTrue(task.isOverdue());
		}

		// Task should never be ongoing (starts in year 2100, never ongoing).
		TEST_METHOD(TaskIsNotOngoing) {
			Task task;
			task.setBegin(QDateTime(QDate(2100, 12, 31),QTime(23, 59, 59)));
			Assert::IsFalse(task.isOngoing());
		}

		// Task should always be ongoing (task started in year 2010, always ongoing).
		TEST_METHOD(TaskIsOngoing) {
			Task task;
			task.setBegin(QDateTime(QDate(2010, 1, 1), QTime(1, 1, 1)));
			Assert::IsTrue(task.isOngoing());
		}

		TEST_METHOD(TaskIsOngoingBecauseNotOverdue) {
			Task task;
			task.setBegin(QDateTime(QDate(2010, 1, 1), QTime(1, 1, 1)));
			task.setEnd(QDateTime(QDate(2100, 12, 31),QTime(23, 59, 59)));
			Assert::IsTrue(task.isOngoing());
			Assert::IsFalse(task.isOverdue());
		}

		TEST_METHOD(TaskIsNotOngoingBecauseOverdue) {
			Task task;
			task.setBegin(QDateTime(QDate(2010, 1, 1), QTime(1, 1, 1)));
			task.setEnd(QDateTime(QDate(2010, 1, 2), QTime(1, 1, 1)));
			Assert::IsFalse(task.isOngoing());
			Assert::IsTrue(task.isOverdue());
		}

	};
}