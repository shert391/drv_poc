#include "rtcore.h"

void rtcore::read_kernel_memory(uintptr_t pAddress, OUT PDWORD pOut, size_t size)
{
	read_write_request request{ 0 };
	request.pAddress = pAddress;
	request.Size = size;
	request.Offset = 0;

	if (!DeviceIoControl(HProviderDriver,
		IOCTL_READ,
		&request,
		sizeof(read_write_request),
		&request,
		sizeof(read_write_request),
		nullptr,
		nullptr)) {
		LogError("RtCore read_vm fail!");
	}

	*pOut = request.Data;

	LogSuccess("RtCore successfully read_vm 0x%X from 0x%p", request.Data, (PVOID)pAddress);
}

void rtcore::write_kernel_memory(uintptr_t pAddress, DWORD value, size_t size, OUT PDWORD pOldValue)
{
	if (pOldValue)
		read_kernel_memory(pAddress, pOldValue, size);

	read_write_request request{ 0 };
	request.pAddress = pAddress;
	request.Data = value;
	request.Size = size;
	request.Offset = 0;

	if (!DeviceIoControl(HProviderDriver,
		IOCTL_WRITE,
		&request,
		sizeof(read_write_request),
		nullptr,
		0,
		nullptr,
		nullptr)) {
		LogError("RtCore write_vm fail!");
	}

	LogSuccess("RtCore successfully write_vm 0x%X in 0x%p", value, (PVOID)pAddress);
}
