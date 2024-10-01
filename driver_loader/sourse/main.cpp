#include "ntiomsi/ntiomsi.h"
#include "rtcore/rtcore.h"
#include "utils/utils.h"
#include "dse_killer.h"
#include <windows.h>
#include <iostream>

int main()
{
	ntiomsi ntiomsi;
	rtcore rtcore;
	dse_killer::load_unsigned_driver(&rtcore, get_current_directory() / L"driver_test.sys");
}