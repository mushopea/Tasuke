//@author A0096863M
#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QTimer>
#include "Task.h"

// Class for the notification timer.
class NotificationManager : public QObject {
	Q_OBJECT
private:
	QTimer timer;
	Task nextTask;

	NotificationManager();
	NotificationManager(const NotificationManager& old);
	const NotificationManager& operator=(const NotificationManager& old);
	~NotificationManager();

public:
	static NotificationManager &instance();
	void init(void* storage);
	void scheduleNotification(Task task);
	
public slots:
	void handleTimeout();
};

#endif