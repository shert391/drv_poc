#pragma once
#include "sourse/utils/ntstatus.h"
#include "sourse/utils/debug.h"
#include <filesystem>
#include <windows.h>
#include <string>
#include <random>

#define SeLoadDriverPrivilege 0xa

using namespace std;
using namespace std::filesystem;

wstring random_string(size_t length);

void unload_driver(path drvPath);
void load_driver(path drvPath);

path get_system32_drivers_path();
path get_drivers_storage_path();