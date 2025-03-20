#include <Windows.h>
#include <Psapi.h>
#include <vector>
#include <string>
#include <algorithm>
#include <shellapi.h>
#pragma comment(lib, "Psapi.lib")

// �������������б�ȫСд�� List of whitelist process names (all lowercase)
const std::vector<std::string> whitelist = {
    "cherry studio.exe",
    "explorer.exe", //Windows��Դ������ Windows Explorer
    "taskmgr.exe",  //��������� task manager
    "lockapp.exe",  //���� Lock screen
    "msedge.exe",   //edge�����  Microsoft Edge Browser
    "devenv.exe"    //VS
};

DWORD g_currentProcessID = 0;
HWND g_myConsoleWindow = nullptr;

// ��ȡ��������ȫСд��  Get process name (all lowercase)
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

// ϵͳ���ڼ��  System window detection

bool IsSystemWindow(HWND hwnd) {
    if (hwnd == GetDesktopWindow() || hwnd == GetShellWindow())
        return true;

    char classname[256];
    GetClassNameA(hwnd, classname, sizeof(classname));
    return (strcmp(classname, "Progman") == 0 ||
        strcmp(classname, "WorkerW") == 0 ||
        strcmp(classname, "Shell_TrayWnd") == 0);
}

// ��ǿ��������⣨��������ID���ҷ����� Enhance whitelist detection (including process ID self-protection)
bool IsProcessAllowed(DWORD processID) {
    // ʼ������ǰ����  Always allow the current process
    if (processID == g_currentProcessID)
        return true;

    // ��ȡ���������ȶ԰�����  Get the process name and compare it to the whitelist
    std::string procName = GetProcessNameLower(processID);
    for (const auto& allowed : whitelist) {
        if (procName == allowed)
            return true;
    }
    return false;
}
//Needs administrator privileges to run  ��Ҫ����ԱȨ������
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
            MessageBoxW(NULL, L"��Ҫ����ԱȨ�޲��ܼ������г���", L"Ȩ�޲���", MB_ICONERROR);  //Administrator privileges are required to continue running the program, and the 'L' permission is insufficient
        }
        exit(1);
    }
}

int main() {
    if (!IsRunAsAdmin()) {
        ElevatePrivileges();
        return 0;
    }
    printf("�ѻ�ù���ԱȨ��\n");  //Obtained administrator privileges
    g_currentProcessID = GetCurrentProcessId();
    g_myConsoleWindow = GetConsoleWindow(); // ��ȡ����̨���ھ��   Get console window handle
    printf("[*] רעģʽ��������PID=%d��\n", g_currentProcessID);  // [*] Focus mode has been activated(PID = % d)
    printf("[����̨���] 0x%p\n", g_myConsoleWindow); //[Console Handle] 0x% p
    for (const auto& name : whitelist)
        printf("���������̣�%s\n", name.c_str());  //Whitelist process:% s
    printf("\n��Ctrl+C�˳�...\n");   //Press Ctrl+C to exit

    // �����ѭ��  Main monitoring loop
    while (true) {
        HWND foregroundWindow = GetForegroundWindow();
        if (foregroundWindow && !IsSystemWindow(foregroundWindow)) {
            // �����Լ��Ŀ���̨����    Skip your own console window
            if (foregroundWindow == g_myConsoleWindow)
                continue;

            DWORD processID = 0;
            GetWindowThreadProcessId(foregroundWindow, &processID);

            if (!IsProcessAllowed(processID)) {
                // ��ʾ��ϸ��Ϣ�������    Display detailed information for easy debugging
                char windowTitle[256];
                GetWindowTextA(foregroundWindow, windowTitle, sizeof(windowTitle));
                printf("���ش��ڣ�PID=%d NAME=%s TITLE=%s\n",
                    processID, GetProcessNameLower(processID).c_str(), windowTitle);   //Intercept window: PID=%d NAME=%s TITLE=%s

                PostMessage(foregroundWindow, WM_CLOSE, 0, 0);
            }
        }
        Sleep(500);
    }
}
