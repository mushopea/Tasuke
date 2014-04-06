#include <glog/logging.h>
#include <QtWidgets/QApplication>
#include "Tasuke.h"

int main(int argc, char *argv[]) {
	google::InitGoogleLogging(argv[0]);
	LOG(INFO) << "Tasuke starting up!";

	QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("Tasuke");
	QCoreApplication::setApplicationName("Tasuke");
	
	Tasuke::instance();

	return app.exec();
}
