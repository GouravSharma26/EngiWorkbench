#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PWSTR pCmdLine, int nCmdShow) {
    // 1. Locate current directory of Launch.exe
    wchar_t modulePath[MAX_PATH];
    DWORD len = GetModuleFileNameW(NULL, modulePath, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) {
        MessageBoxW(NULL, L"Failed to determine launcher executable path.", L"Launch Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    fs::path launcherDir = fs::path(modulePath).parent_path();

    // 2. Resolve target application executable relative to launcher
    // Check same directory first, then bin/ subdirectory
    fs::path targetExe = launcherDir / L"workbench_app.exe";
    if (!fs::exists(targetExe)) {
        targetExe = launcherDir / L"bin" / L"workbench_app.exe";
    }

    if (!fs::exists(targetExe)) {
        std::wstring msg = L"Could not locate the workbench application executable.\n\n"
                           L"Expected path:\n" + (launcherDir / L"workbench_app.exe").wstring() + L"\n"
                           L"or:\n" + (launcherDir / L"bin" / L"workbench_app.exe").wstring() + L"\n\n"
                           L"Please verify that the application was extracted completely and not moved.";
        MessageBoxW(NULL, msg.c_str(), L"PASS Engineering Workbench — Missing Binary", MB_ICONERROR | MB_OK);
        return 2;
    }

    fs::path workingDir = targetExe.parent_path();

    // 3. Prepare command line forwarding
    std::wstring commandLine = L"\"" + targetExe.wstring() + L"\"";
    if (pCmdLine && wcslen(pCmdLine) > 0) {
        commandLine += L" ";
        commandLine += pCmdLine;
    }

    std::vector<wchar_t> cmdBuffer(commandLine.begin(), commandLine.end());
    cmdBuffer.push_back(L'\0');

    // 4. Configure process startup
    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = (nCmdShow > 0) ? nCmdShow : SW_SHOWNORMAL;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    // 5. Spawn workbench_app.exe
    BOOL created = CreateProcessW(
        targetExe.c_str(),
        cmdBuffer.data(),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        workingDir.c_str(),
        &si,
        &pi
    );

    if (!created) {
        DWORD err = GetLastError();
        std::wstring msg = L"Failed to launch workbench application process.\nWindows Error Code: " + std::to_wstring(err);
        MessageBoxW(NULL, msg.c_str(), L"PASS Engineering Workbench — Execution Failed", MB_ICONERROR | MB_OK);
        return static_cast<int>(err);
    }

    // 6. Wait for completion and return exact exit code
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return static_cast<int>(exitCode);
}

