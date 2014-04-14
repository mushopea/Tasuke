//@author A0096863M

#ifndef STORAGESTUB_H
#define STORAGESTUB_H

#include "Storage.h"

class StorageStub : public IStorage {
public:
	StorageStub();
	~StorageStub();
	void saveFile();
	void loadFile();
};

#endif

