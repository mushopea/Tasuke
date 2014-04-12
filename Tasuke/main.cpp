#include <glog/logging.h>
#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>
#include "Tasuke.h"
#include "Constants.h"

//@author A0096836M

void exitIfAlreadyRunning() {
	QSharedMemory sharedMemory;
	sharedMemory.setKey(SHARED_MEMORY_KEY);
	sharedMemory.attach();

	if (!sharedMemory.create(1)) {
		QMessageBox::information(nullptr, NAME_APPLICATION, MSG_ALREADY_RUNNING);
		exit(EXIT_FAILURE);
	}
}

void initLogging(int argc, char *argv[]) {
	Q_UNUSED(argc);

	google::InitGoogleLogging(argv[0]);
	LOG(INFO) << MSG_STARTING_UP;
}

void setOrganizationAndApplicationName() {
	QCoreApplication::setOrganizationName(NAME_ORGANIZATION);
	QCoreApplication::setApplicationName(NAME_APPLICATION);
}

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	exitIfAlreadyRunning();
	initLogging(argc, argv);
	setOrganizationAndApplicationName();;

	Tasuke::instance();

	return app.exec();
}
