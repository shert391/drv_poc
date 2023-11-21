#pragma once
#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

class dse_killer {
public:
	static void disable_dse();
private:
	static void create_file_ntcore_from_binary();
};