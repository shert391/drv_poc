#include "scanner.h"

uintptr_t scanner::sig_scan_section(LPCSTR szSectionName, PIMAGE_DOS_HEADER pBase, LPCSTR sig, size_t sigSize)
{
	PIMAGE_NT_HEADERS64 pNtHeaders = (PIMAGE_NT_HEADERS64)((PBYTE)pBase + pBase->e_lfanew);
	PIMAGE_SECTION_HEADER pSectionHeaders = (PIMAGE_SECTION_HEADER)((PBYTE)pNtHeaders + sizeof(IMAGE_NT_HEADERS64));

	for (size_t i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++, pSectionHeaders++)
		if(!memcmp(pSectionHeaders->Name, szSectionName, strlen(szSectionName)))
			return sig_scan((LPCSTR)pBase + pSectionHeaders->VirtualAddress, sig, sigSize, pSectionHeaders->SizeOfRawData);

	return 0;
}

uintptr_t scanner::sig_scan(LPCSTR pAddress, LPCSTR sig, size_t sigSize, size_t range)
{
	for (size_t i = 0; i < range; i++)
	{
		size_t j{ 0 };

		for (; j < sigSize; j++)
			if (pAddress[i + j] != sig[j] && sig[j] != '\x00')
				break;

		if (j == sigSize)
			return (uintptr_t)(&pAddress[i]);
	}

	return 0;
}
