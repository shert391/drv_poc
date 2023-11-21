#pragma once
#include <windows.h>
#include <string>
#include <random>

using namespace std;

wstring random_string(size_t length) {
    const std::wstring CHARACTERS = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    random_device random_device;
    mt19937 generator(random_device());
    uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::wstring random_string;

    for (std::size_t i = 0; i < length; ++i)
        random_string += CHARACTERS[distribution(generator)];

    return random_string;
}

wstring get_system32_path() {
    wchar_t szSystem32Directory[MAX_PATH]{ 0 };

    GetSystemDirectory(szSystem32Directory, sizeof(szSystem32Directory));

    return wstring(szSystem32Directory) + L"\\";
}