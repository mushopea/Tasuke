#include <glog/logging.h>
#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>
#include "Tasuke.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QSharedMemory sharedMemory;
	sharedMemory.setKey("Tasuke");
	sharedMemory.attach();

	if (!sharedMemory.create(1)) {
		QMessageBox::information(nullptr, "Tasuke", "Tasuke is already running! Open Tasuke using ctrl+space");
		return EXIT_FAILURE;
	}

	google::InitGoogleLogging(argv[0]);
	LOG(INFO) << "Tasuke starting up!";

	QCoreApplication::setOrganizationName("Tasuke");
	QCoreApplication::setApplicationName("Tasuke");
	
	Tasuke::instance();

	return app.exec();
}
