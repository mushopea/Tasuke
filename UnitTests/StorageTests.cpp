#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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

			Tasuke::instance().runCommand("add DESCRIPTION IN UPPERCASE #tag1 #tag2 #TAGCASE");
			Tasuke::instance().runCommand("add description in lowercase #tag2 #tag3 #tagcase");
			Tasuke::instance().runCommand("add DeScRiPtIoN iN iReGuLaR cAsE #tag3 #tag1");
			Tasuke::instance().runCommand("add existing description #tag2 #tag1");
		}

		TEST_METHOD_CLEANUP(deinit) {
			delete storage;
			storage = nullptr;
		}
		
		/********** Tests for TASK class **********/

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

		// Task should be ongoing and not overdue (task starts 2010 Jan, ends 2100 Dec).
		TEST_METHOD(TaskIsOngoingBecauseNotOverdue) {
			Task task;
			task.setBegin(QDateTime(QDate(2010, 1, 1), QTime(1, 1, 1)));
			task.setEnd(QDateTime(QDate(2100, 12, 31),QTime(23, 59, 59)));
			Assert::IsTrue(task.isOngoing());
			Assert::IsFalse(task.isOverdue());
		}

		// Task should not be ongoing, and is overdue (task starts 2010 Jan, ends 2010 Jan).
		TEST_METHOD(TaskIsNotOngoingBecauseOverdue) {
			Task task;
			task.setBegin(QDateTime(QDate(2010, 1, 1), QTime(1, 1, 1)));
			task.setEnd(QDateTime(QDate(2010, 1, 2), QTime(1, 1, 1)));
			Assert::IsFalse(task.isOngoing());
			Assert::IsTrue(task.isOverdue());
		}

		/********** Tests for STORAGE class **********/
		
		TEST_METHOD(StorageSearchByDescription) {
			Assert::AreEqual(storage->searchByDescription("description").size(), 4);
			Assert::AreEqual(storage->searchByDescription("DESCRIPTION").size(), 4);
			Assert::AreEqual(storage->searchByDescription("description", Qt::CaseSensitive).size(), 2);
			Assert::AreEqual(storage->searchByDescription("DeScRiPtIoN", Qt::CaseSensitive).size(), 1);
			Assert::AreEqual(storage->searchByDescription("DeScRiPtIoN").size(), 4);
			Assert::AreEqual(storage->searchByDescription("nonexistent description").size(), 0);
		}
		
		TEST_METHOD(StorageSearchByTag) {
			// Case sensitive tests
			Assert::AreEqual(storage->searchByTag("tag1", Qt::CaseSensitive).size(), 3);
			Assert::AreEqual(storage->searchByTag("tagcase", Qt::CaseSensitive).size(), 1);
			Assert::AreEqual(storage->searchByTag("TAGCASE", Qt::CaseSensitive).size(), 1);
			Assert::AreEqual(storage->searchByTag("tag3", Qt::CaseSensitive).size(), 2);

			// Case insensitive tests
			Assert::AreEqual(storage->searchByTag("TAGCASE").size(), 2);
			Assert::AreEqual(storage->searchByTag("tagcase").size(), 2);
		}
	};
}