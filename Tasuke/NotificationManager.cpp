#include <QDateTime>
#include "NotificationManager.h"
#include "Tasuke.h"
#include "Constants.h"
#include "Exceptions.h"
#include "Storage.h"

//@author A0096863M

NotificationManager::NotificationManager() {
	connect(&timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));

}

NotificationManager::~NotificationManager() {

}

NotificationManager& NotificationManager::instance() {
	static NotificationManager *instance = 0;

	if (instance == 0) {
		instance = new NotificationManager();
		return *instance;
	} else {
		return *instance;
	}
}

void NotificationManager::init(void* storage) {
	assert(storage != nullptr);

	try {
		Task next = static_cast<IStorage*>(storage)->getNextUpcomingTask();	
		scheduleNotification(next);
	} catch (ExceptionNoMoreTasks &exception) {
		// there's no upcoming task
		// we don't have to do anything
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
	init(&Tasuke::instance().getStorage());
}