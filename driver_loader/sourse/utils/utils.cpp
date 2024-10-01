#include "utils.h"

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

void load_driver(path drvPath)
{
    path scmDataBaseKey = "System\\CurrentControlSet\\Services\\" / drvPath.filename().replace_extension();
    const wchar_t* szImagePath = L"ImagePath";  
    const wchar_t* szType = L"Type"; DWORD vType = 1;

    HKEY hKey{ 0 };
    RegCreateKey(HKEY_LOCAL_MACHINE, scmDataBaseKey.c_str(), &hKey);
    RegSetValueEx(hKey, szType, 0, REG_DWORD, (PBYTE)&vType, sizeof(DWORD));

    wstring objPath = L"\\??\\" + drvPath.wstring();
    RegSetValueEx(hKey, szImagePath, 0, REG_EXPAND_SZ, (PBYTE)objPath.c_str(), wcslen(objPath.c_str()) * sizeof(wchar_t) + 2);

    DebugLog("Regedit Service Data Base driver key is configured: HKEY_LOCAL_MACHINE\\%ls", scmDataBaseKey.c_str());
    RegCloseKey(hKey);

    UNICODE_STRING unicodeKey{ 0 };
    scmDataBaseKey = "\\Registry\\Machine" / scmDataBaseKey;
    RtlInitUnicodeString(&unicodeKey, (PWSTR)scmDataBaseKey.c_str());

    BOOLEAN oldValue{ 0 };
    RtlAdjustPrivilege(SeLoadDriverPrivilege, true, false, &oldValue);
    NTSTATUS ntstatus = NtLoadDriver(&unicodeKey);

    if (ntstatus == STATUS_IMAGE_ALREADY_LOADED ||
        ntstatus == STATUS_OBJECT_NAME_COLLISION) {
        LogSuccess("The %ls driver is already loaded!", drvPath.filename().c_str());
        return;
    }

    CheckNtStatus(ntstatus);
    LogSuccess("The %ls driver is loaded", drvPath.filename().c_str());
}

void unload_driver(path drvPath) {
    path scmDataBaseKey = "SYSTEM\\CurrentControlSet\\Services\\" / drvPath.filename().replace_extension();

    UNICODE_STRING unicodeKey{ 0 };
    path scmDataBaseKeyEx = "\\Registry\\Machine" / scmDataBaseKey;
    RtlInitUnicodeString(&unicodeKey, (PWSTR)scmDataBaseKeyEx.c_str());

    BOOLEAN oldValue{ 0 };
    RtlAdjustPrivilege(SeLoadDriverPrivilege, true, false, &oldValue);
    NTSTATUS ntstatus = NtUnloadDriver(&unicodeKey);
    LogSuccess("The %ls driver unloaded!", drvPath.filename().c_str());

    RegDeleteKey(HKEY_LOCAL_MACHINE, scmDataBaseKey.c_str());

    DebugLog("Regedit Service Data Base driver key clear: HKEY_LOCAL_MACHINE\\%ls", scmDataBaseKey.c_str());
}

path get_system32_drivers_path() {
    wchar_t szSystem32Directory[MAX_PATH]{};
    GetSystemDirectory(szSystem32Directory, MAX_PATH);
    return path(szSystem32Directory) / L"drivers";
}

path get_current_directory() {
    wchar_t szCurrentDirectory[MAX_PATH]{};
    GetCurrentDirectory(MAX_PATH, szCurrentDirectory);
    return path(szCurrentDirectory);
}