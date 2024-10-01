#include "rtcore.h"

void rtcore::read_kernel_virtualmemory(uintptr_t pAddress, size_t size, OUT PVOID pOut)
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

	memcpy(pOut, &request.Data, size);
}

void rtcore::write_kernel_virtualmemory(uintptr_t pAddress, PVOID pWriteData, size_t size, OUT PVOID pOldData)
{
	if (pOldData)
		read_kernel_virtualmemory(pAddress, size, pOldData);

	read_write_request request{ 0 };
	request.pAddress = pAddress;
	request.Size = size;
	request.Offset = 0;
	memcpy(&request.Data, pWriteData, size);

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
}
