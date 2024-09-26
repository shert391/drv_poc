#include "provider.h"

void provider::create_driver_file_from_binary()
{
	if (exists(SzProviderDriverFilePath))
		return;

	HANDLE hFile = CreateFileW(SzProviderDriverFilePath.c_str(),
		GENERIC_ALL,
		0,
		nullptr,
		CREATE_NEW,
		0,
		nullptr);
	CheckHandleEx(hFile, "%ls", SzProviderDriverFilePath.c_str());

	WriteFile(hFile, BinaryData.PBinary, BinaryData.Size, nullptr, nullptr);

	DebugLog("%ls created!", SzProviderDriverFilePath.c_str());
	CloseHandle(hFile);
}

void provider::init() {
	create_driver_file_from_binary();
	load_driver(SzProviderDriverFilePath);
	LPCSTR deviceObjectName;
	_pathToStrA(path("\\??\\") / SzDeviceObjectName, deviceObjectName);
	HProviderDriver = CreateFileA(deviceObjectName,
								  GENERIC_ALL,
								  0,
								  nullptr,
								  OPEN_EXISTING,
								  0,
								  0);
}

void provider::release()
{
	CloseHandle(HProviderDriver);

	unload_driver(SzProviderDriverFilePath);

	if (!DeleteFile(SzProviderDriverFilePath.c_str()))
		LogError("DeleteFile %ls fail!", SzProviderDriverFilePath.c_str());

	DebugLog("%ls deleted!", SzProviderDriverFilePath.c_str());
}
