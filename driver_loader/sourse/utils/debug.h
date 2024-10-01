#pragma once

#include <windows.h>
#include "ntdll.h"

#pragma region Format
#define FormatGetLastError \
	wchar_t error[256]{ 0 }; \
	int32_t code = GetLastError(); \
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK, \
		NULL, GetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), \
		error, (sizeof(error) / sizeof(wchar_t)), nullptr);

#define FormatNtStatus(ntstatus) \
	wchar_t error[256]{ 0 }; \
	int32_t code = GetLastError(); \
	FormatMessageW(FORMAT_MESSAGE_FROM_HMODULE, LoadLibrary(L"ntdll.dll"), ntstatus, \
		MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), error, (sizeof(error) / sizeof(wchar_t)), nullptr);

#define PrintError(...) \
		{ \
			printf("\033[1;31m[-] "); \
			printf(__VA_ARGS__); \
			printf("\033[1;0m\n"); \
		}

#define PrintSuccess(...) \
		{ \
			printf("\033[1;32m[+] "); \
			printf(__VA_ARGS__); \
			printf("\033[1;0m\n"); \
		}
#pragma endregion

#pragma region HandleCheck
#define CheckHandleEx(handle, message, ...) \
	if (handle == INVALID_HANDLE_VALUE) { \
		FormatGetLastError \
		PrintError(##message##" GetLastError is %#x => %ls", __VA_ARGS__, code, error) \
		DbgBreakPoint(); \
	}

#define CheckHandle(handle) \
	if (handle == INVALID_HANDLE_VALUE) { \
		FormatGetLastError \
		PrintError("GetLastError is %#x => %ls", code, error) \
		DbgBreakPoint(); \
	}

#define CheckNtStatus(ntstatus) \
	if(!NT_SUCCESS(ntstatus)) \
	{ \
		FormatNtStatus(ntstatus) \
		PrintError("NTSTATUS is %#x => %ls", ntstatus, error) \
		DbgBreakPoint(); \
	}
#pragma endregion

#pragma region Debug
#ifdef _DEBUG
#define DebugLog(message, ...) \
		printf("[D] "##message##"\n", __VA_ARGS__)								 
#else
#define DebugLog(message, ...)
#endif
#pragma endregion

#pragma region Loging
#define PrintLastError \
		FormatGetLastError \
		PrintError("GetLastError is %#x => %ls", code, error) \
		DbgBreakPoint();

#define LogError(...) \
		{ PrintError(__VA_ARGS__) \
		DbgBreakPoint(); }

#define LogSuccess(...) \
		PrintSuccess(__VA_ARGS__)

#define Log(message, ...) \
		printf("[~] "##message##"\n", __VA_ARGS__)	
#pragma endregion
