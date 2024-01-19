#pragma once
#include "rtcore_binary.h"
#include "utils/debug.h"
#include "utils/utils.h"
#include <windows.h>
#include <iostream>
#include "config.h"

#define IOCTL_WRITE_VM CTL_CODE(0x8000, 0x813, METHOD_BUFFERED, FILE_ANY_ACCESS) // 0x8000204C
#define IOCTL_READ_VM CTL_CODE(0x8000, 0x812, METHOD_BUFFERED, FILE_ANY_ACCESS) // 0x80002048

struct read_write_vm_request {
	char Unkown0[8];
	uintptr_t pAddress;
	char Unkown1[4];
	unsigned int Offset;
	DWORD Size;
	DWORD Data;
	char Unkown2[16];
};

class rtcore {
private:
	inline static HANDLE _hRtCore;

	static void create_file_ntcore_from_binary();
public:
	static void init();
	static void release();
	static void read_vm(uintptr_t pAddress, OUT PDWORD pOut, size_t size);
	static void write_vm(uintptr_t pAddress, DWORD value, size_t size, OUT PDWORD oldValue = nullptr);
};