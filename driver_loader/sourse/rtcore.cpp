#include "rtcore.h"

void rtcore::create_file_ntcore_from_binary()
{
	path path = get_drivers_storage_path() / RTCORE_NAME;

	if (exists(path))
		return;

	HANDLE hFile = CreateFileW(path.c_str(),
		GENERIC_ALL,
		0,
		nullptr,
		CREATE_NEW,
		0,
		nullptr);
	CheckHandleEx(hFile, "%ls", path.c_str());

	WriteFile(hFile, g_aRtCoreSysBinary, sizeof(g_aRtCoreSysBinary), nullptr, nullptr);

	DebugLog("%ls created!", path.c_str());
	CloseHandle(hFile);
}

void rtcore::release() {
	CloseHandle(_hRtCore);

	path path = get_drivers_storage_path() / RTCORE_NAME;
	unload_driver(path);

	if (!DeleteFile(path.c_str()))
		LogError("DeleteFile %ls fail!", path.c_str());

	DebugLog("%ls deleted!", path.c_str());
}

void rtcore::init()
{
	create_file_ntcore_from_binary();
	load_driver(get_drivers_storage_path() / RTCORE_NAME);
	_hRtCore = CreateFileA("\\\\.\\RtCore64", 
		GENERIC_ALL, 
		0, 
		nullptr, 
		OPEN_EXISTING, 
		0, 
		0);
}

void rtcore::write_vm(uintptr_t pAddress, DWORD value, size_t size, OUT PDWORD oldValue)
{
	if (oldValue)
		read_vm(pAddress, oldValue, size);

	read_write_vm_request request{ 0 };
	request.pAddress = pAddress;
	request.Data = value;
	request.Size = size;
	request.Offset = 0;

	if (!DeviceIoControl(_hRtCore,
		IOCTL_WRITE_VM,
		&request,
		sizeof(read_write_vm_request),
		nullptr,
		0,
		nullptr,
		nullptr)) {
		LogError("RtCore write_vm fail!");
	}
	
	LogSuccess("RtCore successfully write_vm 0x%X in 0x%p", value, (PVOID)pAddress);
}

void rtcore::read_vm(uintptr_t pAddress, OUT PDWORD pOut, size_t size)
{
	read_write_vm_request request{ 0 };
	request.pAddress = pAddress;
	request.Size = size;
	request.Offset = 0;

	if (!DeviceIoControl(_hRtCore,
		IOCTL_READ_VM,
		&request,
		sizeof(read_write_vm_request),
		&request,
		sizeof(read_write_vm_request),
		nullptr,
		nullptr)) {
		LogError("RtCore read_vm fail!");
	}

	*pOut = request.Data;

	LogSuccess("RtCore successfully read_vm 0x%X from 0x%p", request.Data, (PVOID)pAddress);
}

