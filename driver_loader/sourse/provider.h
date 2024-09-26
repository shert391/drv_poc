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
	void init();
	void release();

	virtual void read_kernel_memory(uintptr_t pAddress, OUT PDWORD pOut, size_t size) = 0;
	virtual void write_kernel_memory(uintptr_t pAddress, DWORD value, size_t size, OUT PDWORD pOldValue = nullptr) = 0;

protected:
	provider(PCSTR szDeviceObjectName, PCSTR szProviderDriverFileName, binary_data binaryData) {
		BinaryData = binaryData;
		SzDeviceObjectName = szDeviceObjectName;
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