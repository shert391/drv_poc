#pragma once
#include "utils/scanner.h"
#include "utils/utils.h"
#include "utils/debug.h"
#include "provider.h"
#include <windows.h>
#include <config.h>
#include <iostream>
#include <string>

using namespace std;

class dse_killer {
public:
	static void load_unsigned_driver(provider* provider, path driverPath);
private:
	static uintptr_t get_ci_image_base_in_kernel();
	static uintptr_t get_g_cioptions_rva();
	static uintptr_t map_ci_into_memory();
};