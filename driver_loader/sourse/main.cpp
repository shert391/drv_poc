#include "utils/utils.h"
#include "dse_killer.h"
#include <windows.h>
#include <iostream>

int main()
{
	dse_killer::load_unsigned_driver(get_drivers_storage_path() / L"driver_test.sys");
}