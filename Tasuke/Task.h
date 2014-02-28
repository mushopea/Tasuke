#ifndef TASK_H
#define TASK_H

#include <QDateTime>
#include <vector>
#include <string>

class Task {
private:
	std::string description;
	std::vector<std::string> tags;
	QDateTime begin;
	QDateTime end;

public:
	Task();
	~Task();

	void setDescription(std::string& _description);
	std::string getDescription() const;

	void addTag(std::string& tag);
	void removeTag(std::string& tag);

	void setBegin(QDateTime& _begin);
	QDateTime getBegin() const;

	void setEnd(QDateTime& _end);
	QDateTime getEnd() const;
};

#endif