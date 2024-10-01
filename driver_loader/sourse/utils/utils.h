#pragma once
#include "sourse/utils/ntstatus.h"
#include "sourse/utils/debug.h"
#include <filesystem>
#include <windows.h>
#include <string>
#include <random>

#define SeLoadDriverPrivilege 0xa
#define _pathToStrA(pathIn, out) \
	string buff = (pathIn).string(); \
	out = buff.c_str();

using namespace std;
using namespace std::filesystem;

struct VIRTUAL_ADDRESS {
public:
	VIRTUAL_ADDRESS(uintptr_t virtualAddress) {
		Pml4e_Index = (virtualAddress >> 39) & 0x1FF;
		Pdpe_Index = (virtualAddress >> 30) & 0x1FF;
		Pde_Index = (virtualAddress >> 21) & 0x1FF;
		Pte_Index = (virtualAddress >> 12) & 0x1FF;
		Pf_4k_Offset = virtualAddress & 0xFFF;
	}

	WORD Pml4e_Index;
	WORD Pdpe_Index;
	WORD Pde_Index;
	WORD Pte_Index;
	WORD Pf_4k_Offset;
};

wstring random_string(size_t length);

void unload_driver(path drvPath);
void load_driver(path drvPath);

path get_system32_drivers_path();
path get_current_directory();