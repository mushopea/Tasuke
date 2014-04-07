#include "stdafx.h"
#include <glog/logging.h>
#include "StorageStub.h"

StorageStub::StorageStub()
{
	LOG(INFO) << "StorageStub created destroyed";
}

StorageStub::~StorageStub()
{
	LOG(INFO) << "StorageStub instance destroyed";
}

void StorageStub::saveFile() {
	LOG(INFO) << "Stub saving file";
}

void StorageStub::loadFile() {
	LOG(INFO) << "Stub loading file";
}