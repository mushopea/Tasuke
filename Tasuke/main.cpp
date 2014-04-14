//@author A0096836M

#include <glog/logging.h>
#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>
#include "Tasuke.h"
#include "Constants.h"

// Exits the program if another instance of Tasuke is already running
// Run this right at the start so you can fail fast
void exitIfAlreadyRunning() {
	QSharedMemory sharedMemory;
	sharedMemory.setKey(SHARED_MEMORY_KEY);
	sharedMemory.attach();

	if (!sharedMemory.create(1)) {
		QMessageBox::information(nullptr, NAME_APPLICATION,
			MSG_TASUKE_ALREADY_RUNNING);
		exit(EXIT_FAILURE);
	}
}

// Sets up the glog logger. Run this right at the start so you can
// log as early as possible
void initLogging(int argc, char *argv[]) {
	Q_UNUSED(argc);

	google::InitGoogleLogging(argv[0]);
	LOG(INFO) << MSG_TASUKE_STARTING_UP;
}

// Sets the globals for name and organization in Qt. These constants
// are used in Qt for save files and etc.
void setOrganizationAndApplicationName() {
	QCoreApplication::setOrganizationName(NAME_ORGANIZATION);
	QCoreApplication::setApplicationName(NAME_APPLICATION);
}

// The entry point for the program.
int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	exitIfAlreadyRunning();
	initLogging(argc, argv);
	setOrganizationAndApplicationName();;

	// Create tasuke for the first and only time
	Tasuke::instance();

	return app.exec();
}
