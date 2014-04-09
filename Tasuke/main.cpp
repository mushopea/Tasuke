#include <glog/logging.h>
#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>
#include "Tasuke.h"

//@author A0096836M

const char* const TASUKE = "Tasuke";
const char* const NAME_ORGANIZATION = TASUKE;
const char* const NAME_APPLICATION = TASUKE;
const char* const SHARED_MEMORY_KEY = TASUKE;

const char* const MSG_STARTING_UP = "Tasuke starting up!";
const char* const MSG_ALREADY_RUNNING =  "Tasuke is already running! Open Tasuke using ctrl+space";

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
