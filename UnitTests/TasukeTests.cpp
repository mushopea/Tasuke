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
		
		TEST_METHOD(TasukeBadInput) {
			Assert::ExpectException<ExceptionBadCommand>([] {
				ICommand* command = Interpreter::interpret("bad command blah blah");
				command->run();
			});
			Tasuke::instance().runCommand("bad command blah blah");
			Assert::AreEqual(storage->totalTasks(), 0);
		}

		TEST_METHOD(TasukeAddingTasks) {
			Tasuke::instance().runCommand("add do homework");
			Assert::AreEqual(storage->totalTasks(), 1);

			Task task = storage->getTask(0);
			Assert::AreEqual(task.getDescription(), QString("do homework"));
			Assert::IsFalse(task.getBegin().isValid());
			Assert::IsFalse(task.getEnd().isValid());
			Assert::AreEqual(task.getTags().size(), 0);
		}

		TEST_METHOD(TasukeRemovingTasks) {
			Tasuke::instance().runCommand("add do homework");
			Tasuke::instance().runCommand("add buy eggs");
			Tasuke::instance().runCommand("add watch anime");
			Tasuke::instance().runCommand("remove 2");
			Assert::AreEqual(storage->totalTasks(), 2);
		}

		TEST_METHOD(TasukeEditingTasks) {
			Tasuke::instance().runCommand("add do homework");
			Tasuke::instance().runCommand("add buy eggs");
			Tasuke::instance().runCommand("add watch anime");
			Tasuke::instance().runCommand("edit 2 buy milk @ 2pm - 3pm #shopping");
			Assert::AreEqual(storage->totalTasks(), 3);

			Task task = storage->getTask(0);
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

		TEST_METHOD(TasukeUndoingTasks) {
			for (int i=0; i<MAX_TASKS; i++) {
				Tasuke::instance().runCommand(QString("add task %1").arg(i));
			}
			Assert::AreEqual(storage->totalTasks(), MAX_TASKS);

			for (int i=0; i<MAX_TASKS; i++) {
				Tasuke::instance().runCommand("undo");
			}
			Assert::AreEqual(storage->totalTasks(), 0);
		}

		TEST_METHOD(TasukeRedoingTasks) {
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

		// Musho spelling tests
		// The correct spelling partition
		TEST_METHOD(SpellCorrectSpelling) {
			
			// rarity
			Assert::IsTrue(Tasuke::instance().spellCheck("the")); // A very common word
			Assert::IsTrue(Tasuke::instance().spellCheck("island")); // An uncommon word
			Assert::IsTrue(Tasuke::instance().spellCheck("ophthalmology")); // A rare word

			// word length
			Assert::IsTrue(Tasuke::instance().spellCheck("a")); // Single letter word
			Assert::IsTrue(Tasuke::instance().spellCheck("awe")); // Short word
			Assert::IsTrue(Tasuke::instance().spellCheck("abstract")); // Medium length word
			Assert::IsTrue(Tasuke::instance().spellCheck("anthropomorphic")); // Long word

			// punctuation
			Assert::IsFalse(Tasuke::instance().spellCheck("youre")); // A phrase that needs punctuation
			Assert::IsTrue(Tasuke::instance().spellCheck("you're")); // A phrase that needs punctuation

			// popular words
			Assert::IsTrue(Tasuke::instance().spellCheck("fuck")); // A popular swear word
		}

		// The incorrect spelling partition
		TEST_METHOD(SpellIncorrectSpelling) {
			Assert::IsFalse(Tasuke::instance().spellCheck("unneccessary")); // A commonly mispelled word
			Assert::IsFalse(Tasuke::instance().spellCheck("asdfghjkl")); // A completely mispelled word
		}
		
		TEST_METHOD(SpellCapitalisedProperWords) {
			Assert::IsTrue(Tasuke::instance().spellCheck("John")); // A proper name
			Assert::IsFalse(Tasuke::instance().spellCheck("Asdfghjkl")); // A garbage capitalised word
		}

		TEST_METHOD(SpellNumberInitialWords) {
			Assert::IsTrue(Tasuke::instance().spellCheck("9pm")); // Time
		}

		// To ensure command words are all considered correct.
		TEST_METHOD(SpellTasukeCommandWords) {

			//non words
			Assert::IsTrue(Tasuke::instance().spellCheck("ls"));
			Assert::IsTrue(Tasuke::instance().spellCheck("nd"));
			Assert::IsTrue(Tasuke::instance().spellCheck("rm"));

			//words
			Assert::IsTrue(Tasuke::instance().spellCheck("settings"));
			Assert::IsTrue(Tasuke::instance().spellCheck("add"));
		}

	};
}