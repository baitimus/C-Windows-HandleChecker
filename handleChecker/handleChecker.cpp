#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <Psapi.h>
#include <ntstatus.h>
#include <winternl.h>
#include <TlHelp32.h>

struct ProcessInfo {
    DWORD pid;
    std::wstring name;
    std::wstring path;
    DWORD parentPid;
    DWORD threadCount;
};

ProcessInfo GetDetailedProcessInfo(DWORD pid) {
    ProcessInfo info = { pid, L"", L"", 0, 0 };

    // Get process path
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess) {
        WCHAR path[MAX_PATH];
        if (GetModuleFileNameEx(hProcess, NULL, path, MAX_PATH)) {
            info.path = path;
        }
        CloseHandle(hProcess);
    }

    // Get parent PID and thread count
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W processEntry = { sizeof(PROCESSENTRY32W) };
        if (Process32FirstW(snapshot, &processEntry)) {
            do {
                if (processEntry.th32ProcessID == pid) {
                    info.name = processEntry.szExeFile;
                    info.parentPid = processEntry.th32ParentProcessID;
                    info.threadCount = processEntry.cntThreads;
                    break;
                }
            } while (Process32NextW(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }

    return info;
}
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#ifndef STATUS_INFO_LENGTH_MISMATCH
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#endif

typedef struct _SYSTEM_HANDLE {
    ULONG ProcessId;
    UCHAR ObjectType;
    UCHAR Flags;
    USHORT Handle;
    PVOID Object;
    ACCESS_MASK GrantedAccess;
    bool ReferencingOurProcess;
} SYSTEM_HANDLE, * PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION {
    ULONG HandleCount;
    SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

typedef NTSTATUS(NTAPI* NtQuerySystemInformationFunc)(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );

std::vector<SYSTEM_HANDLE> GetHandles() {
    auto NtQuerySystemInformation = (NtQuerySystemInformationFunc)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"),
        "NtQuerySystemInformation"
    );

    if (!NtQuerySystemInformation) {
        std::cerr << "Failed to get NtQuerySystemInformation" << std::endl;
        return {};
    }

    ULONG bufferSize = 0x10000;
    PVOID buffer = nullptr;
    NTSTATUS status;

    do {
        buffer = malloc(bufferSize);
        if (!buffer) return {};

        status = NtQuerySystemInformation(16, buffer, bufferSize, &bufferSize);

        if (status == STATUS_INFO_LENGTH_MISMATCH) {
            free(buffer);
            bufferSize *= 2;
        }
        else if (!NT_SUCCESS(status)) {
            free(buffer);
            return {};
        }
    } while (status == STATUS_INFO_LENGTH_MISMATCH);

    PSYSTEM_HANDLE_INFORMATION handleInfo = (PSYSTEM_HANDLE_INFORMATION)buffer;
    std::vector<SYSTEM_HANDLE> handles(handleInfo->Handles, handleInfo->Handles + handleInfo->HandleCount);
    free(buffer);
    return handles;
}

std::vector<SYSTEM_HANDLE> DetectOpenHandlesToProcess(DWORD targetPid) {
    auto handles = GetHandles();
    std::vector<SYSTEM_HANDLE> handlesToPid;

    for (auto& handle : handles) {
        if (handle.ProcessId != targetPid) {
            if (handle.ProcessId == 0 || handle.ProcessId == 4) continue;

            HANDLE processHandle = OpenProcess(PROCESS_DUP_HANDLE, FALSE, handle.ProcessId);
            if (!processHandle) continue;

            HANDLE duplicatedHandle = INVALID_HANDLE_VALUE;
            if (DuplicateHandle(processHandle, (HANDLE)handle.Handle,
                GetCurrentProcess(), &duplicatedHandle,
                0, FALSE, DUPLICATE_SAME_ACCESS)) {

                if (GetProcessId(duplicatedHandle) == targetPid) {
                    std::cout << "Handle " << handle.Handle
                        << " from process " << handle.ProcessId
                        << " is referencing PID " << targetPid << std::endl;
                    handle.ReferencingOurProcess = true;
                    handlesToPid.push_back(handle);
                }
                CloseHandle(duplicatedHandle);
            }
            CloseHandle(processHandle);
        }
    }
    return handlesToPid;
}

std::wstring GetProcessName(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess) return L"";

    WCHAR processName[MAX_PATH] = L"";
    HMODULE hMod;
    DWORD cbNeeded;

    if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
        GetModuleBaseName(hProcess, hMod, processName, sizeof(processName) / sizeof(WCHAR));
    }

    CloseHandle(hProcess);
    return std::wstring(processName);
}

int main() {
    DWORD targetPid;
    std::cout << "Enter target process ID: ";
    std::cin >> targetPid;

    auto handles = DetectOpenHandlesToProcess(targetPid);

    for (const auto& handle : handles) {
        ProcessInfo procInfo = GetDetailedProcessInfo(handle.ProcessId);
        if (!procInfo.name.empty()) {
            std::wcout << L"\nProcess Details:\n"
                << L"Name: " << procInfo.name << L"\n"
                << L"PID: " << procInfo.pid << L"\n"
                << L"Path: " << procInfo.path << L"\n"
                << L"Parent PID: " << procInfo.parentPid << L"\n"
                << L"Thread Count: " << procInfo.threadCount << L"\n"
                << L"Handle Value: " << handle.Handle << L"\n"
                << L"Handle Access: 0x" << std::hex << handle.GrantedAccess << std::dec << L"\n";
        }
    }

    system("pause");
    return 0;
}