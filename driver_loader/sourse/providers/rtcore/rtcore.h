#pragma once
#include "utils.h"
#include "debug.h"
#include "provider.h"
#include "rtcore_binary.h"
#include <windows.h>
#include <iostream>
#include "config.h"

#define RTCORE_PROVIDER_DRIVER_NAME "RTCore64.sys"
#define RTCORE_PROVIDER_SYMBOLIC_OBJECT_NAME "RtCore64"

#define IOCTL_WRITE CTL_CODE(0x8000, 0x813, METHOD_BUFFERED, FILE_ANY_ACCESS) // 0x8000204C
#define IOCTL_READ CTL_CODE(0x8000, 0x812, METHOD_BUFFERED, FILE_ANY_ACCESS) // 0x80002048

struct read_write_request {
	char Unkown0[8];
	uintptr_t pAddress;
	char Unkown1[4];
	unsigned int Offset;
	DWORD Size;
	DWORD Data;
	char Unkown2[16];
};

class rtcore : public provider {
public:
	rtcore() : provider(RTCORE_PROVIDER_SYMBOLIC_OBJECT_NAME, RTCORE_PROVIDER_DRIVER_NAME, binary_data { sizeof(g_aRtCoreSysBinary), g_aRtCoreSysBinary }) {}

	void read_kernel_virtualmemory(uintptr_t pAddress, size_t size, OUT PVOID pOut) override;
	void write_kernel_virtualmemory(uintptr_t pAddress, PVOID pWriteData, size_t size, OUT PVOID pOldData = nullptr) override;
};