#include <QDateTime>
#include "NotificationManager.h"
#include "Tasuke.h"
#include "Constants.h"
#include "Exceptions.h"

NotificationManager::NotificationManager() {
	connect(&timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));

}

NotificationManager::~NotificationManager() {

}

NotificationManager& NotificationManager::instance() {
	static NotificationManager *instance = 0;

	if(instance == 0) {
		// Allocates memory *before* constructor, so Tasuke::instance() will work within a constructor-called method
		instance = (NotificationManager *) ::operator new (sizeof(NotificationManager));
		// Actually runs the constructor now
		new (instance) NotificationManager;
		//instance = new NotificationManager();
		return *instance;
	} else {
		return *instance;
	}
}

void NotificationManager::init() {
	try {
		Task next = Tasuke::instance().getStorage().getNextUpcomingTask();	
		scheduleNotification(next);
	} catch (ExceptionNoMoreTasks &exception) {

	}
}

void NotificationManager::scheduleNotification(Task task) {
	nextTask = task;
	if (timer.isActive()) {
		timer.stop();
	}

	quint64 nextFire = task.getBegin().toMSecsSinceEpoch() - QDateTime::currentDateTime().toMSecsSinceEpoch();
	nextFire -= (10 * MSECS_IN_SECOND * SECONDS_IN_MINUTE);
	
	if (nextFire < 0) {
		return;
	}

	timer.setInterval(nextFire);
	timer.setSingleShot(true);
}

void NotificationManager::handleTimeout() {
	Tasuke::instance().showMessage("Next task: " + nextTask.getDescription() + " begins in 10 minutes.");
	init();
}