//@author A0096863M

#include "stdafx.h"
#include <glog/logging.h>
#include "StorageStub.h"

StorageStub::StorageStub() {
	LOG(INFO) << MSG_STORAGESTUB_INSTANCE_CREATED;
}

StorageStub::~StorageStub() {
	LOG(INFO) << MSG_STORAGESTUB_INSTANCE_DESTROYED;
}

void StorageStub::saveFile() {
	LOG(INFO) << MSG_STORAGESTUB_SAVING_FILE;
}

void StorageStub::loadFile() {
	LOG(INFO) << MSG_STORAGESTUB_LOADING_FILE;
}