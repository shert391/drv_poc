#pragma once
#include <Windows.h>
#include "utils/debug.h"
#include "utils/utils.h"

struct binary_data {
public:
	size_t Size;
	PVOID PBinary;
};

class provider {
public:
	void release();
	virtual void init();

	virtual void read_kernel_virtualmemory(uintptr_t pAddress, size_t size, OUT PVOID pOut) = 0;
	virtual void write_kernel_virtualmemory(uintptr_t pAddress, PVOID pWriteData, size_t size, OUT PVOID pOldData = nullptr) = 0;

protected:
	provider(PCSTR szSymbolicObjectName, PCSTR szProviderDriverFileName, binary_data binaryData) {
		BinaryData = binaryData;
		SzDeviceObjectName = szSymbolicObjectName;
		SzProviderDriverFileName = szProviderDriverFileName;
		SzProviderDriverFilePath = (get_current_directory() / SzProviderDriverFileName).string().c_str();
	}

	HANDLE HProviderDriver = 0;
private:
	binary_data BinaryData;

	PCSTR SzDeviceObjectName;
	path SzProviderDriverFilePath;
	PCSTR SzProviderDriverFileName;

	void create_driver_file_from_binary();
};