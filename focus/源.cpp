#include <Windows.h>
#include <Psapi.h>
#include <vector>
#include <string>
#include <algorithm>
#include <shellapi.h>
#pragma comment(lib, "Psapi.lib")

// 白名单进程名列表（全小写） List of whitelist process names (all lowercase)
const std::vector<std::string> whitelist = {
    "cherry studio.exe",
    "explorer.exe", //Windows资源管理器 Windows Explorer
    "taskmgr.exe",  //任务管理器 task manager
    "lockapp.exe",  //锁屏 Lock screen
    "msedge.exe",   //edge浏览器  Microsoft Edge Browser
    "devenv.exe"    //VS
};

DWORD g_currentProcessID = 0;
HWND g_myConsoleWindow = nullptr;

// 获取进程名（全小写）  Get process name (all lowercase)
std::string GetProcessNameLower(DWORD processID) {
    if (processID == 0) return "";

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (!hProcess) return "";

    char path[MAX_PATH] = { 0 };
    std::string name;
    if (GetModuleFileNameExA(hProcess, NULL, path, MAX_PATH)) {
        const char* pName = strrchr(path, '\\');
        name = (pName != nullptr) ? (pName + 1) : path;
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    }

    CloseHandle(hProcess);
    return name;
}

// 系统窗口检测  System window detection

bool IsSystemWindow(HWND hwnd) {
    if (hwnd == GetDesktopWindow() || hwnd == GetShellWindow())
        return true;

    char classname[256];
    GetClassNameA(hwnd, classname, sizeof(classname));
    return (strcmp(classname, "Progman") == 0 ||
        strcmp(classname, "WorkerW") == 0 ||
        strcmp(classname, "Shell_TrayWnd") == 0);
}

// 增强白名单检测（包含进程ID自我防护） Enhance whitelist detection (including process ID self-protection)
bool IsProcessAllowed(DWORD processID) {
    // 始终允许当前进程  Always allow the current process
    if (processID == g_currentProcessID)
        return true;

    // 获取进程名并比对白名单  Get the process name and compare it to the whitelist
    std::string procName = GetProcessNameLower(processID);
    for (const auto& allowed : whitelist) {
        if (procName == allowed)
            return true;
    }
    return false;
}
//Needs administrator privileges to run  需要管理员权限运行
bool IsRunAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroupSid = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &ntAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &adminGroupSid)) {
        return false;
    }
    CheckTokenMembership(NULL, adminGroupSid, &isAdmin);
    FreeSid(adminGroupSid);

    return isAdmin == TRUE;
}
void ElevatePrivileges() {
    wchar_t appPath[MAX_PATH];
    GetModuleFileNameW(NULL, appPath, MAX_PATH);
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = appPath;
    sei.nShow = SW_NORMAL;
    if (ShellExecuteExW(&sei)) {
        exit(0);
    }
    else {
        if (GetLastError() == ERROR_CANCELLED) {
            MessageBoxW(NULL, L"需要管理员权限才能继续运行程序", L"权限不足", MB_ICONERROR);  //Administrator privileges are required to continue running the program, and the 'L' permission is insufficient
        }
        exit(1);
    }
}

int main() {
    if (!IsRunAsAdmin()) {
        ElevatePrivileges();
        return 0;
    }
    printf("已获得管理员权限\n");  //Obtained administrator privileges
    g_currentProcessID = GetCurrentProcessId();
    g_myConsoleWindow = GetConsoleWindow(); // 获取控制台窗口句柄   Get console window handle
    printf("[*] 专注模式已启动（PID=%d）\n", g_currentProcessID);  // [*] Focus mode has been activated(PID = % d)
    printf("[控制台句柄] 0x%p\n", g_myConsoleWindow); //[Console Handle] 0x% p
    for (const auto& name : whitelist)
        printf("白名单进程：%s\n", name.c_str());  //Whitelist process:% s
    printf("\n按Ctrl+C退出...\n");   //Press Ctrl+C to exit

    // 主监控循环  Main monitoring loop
    while (true) {
        HWND foregroundWindow = GetForegroundWindow();
        if (foregroundWindow && !IsSystemWindow(foregroundWindow)) {
            // 跳过自己的控制台窗口    Skip your own console window
            if (foregroundWindow == g_myConsoleWindow)
                continue;

            DWORD processID = 0;
            GetWindowThreadProcessId(foregroundWindow, &processID);

            if (!IsProcessAllowed(processID)) {
                // 显示详细信息方便调试    Display detailed information for easy debugging
                char windowTitle[256];
                GetWindowTextA(foregroundWindow, windowTitle, sizeof(windowTitle));
                printf("拦截窗口：PID=%d NAME=%s TITLE=%s\n",
                    processID, GetProcessNameLower(processID).c_str(), windowTitle);   //Intercept window: PID=%d NAME=%s TITLE=%s

                PostMessage(foregroundWindow, WM_CLOSE, 0, 0);
            }
        }
        Sleep(500);
    }
}
