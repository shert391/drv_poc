#pragma once
#include <Windows.h>
#include <iterator>
#include <vector>
#include <string>

class scanner
{
public:
	static uintptr_t sig_scan_section(LPCSTR szSectionName, PIMAGE_DOS_HEADER pBase, LPCSTR sig, size_t sigSize);
private:
	static uintptr_t sig_scan(LPCSTR pAddress, LPCSTR sig, size_t sigSize, size_t scanRange);
};