#include <windows.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <thread>
#define BUILD_LIB_SCRIPT "build_lib.bat"
#define BUILD_TESTS_SCRIPT "build_tests.bat"
#define HOTRELOAD_EVENT_NAME "Global\\ReloadEvent"
#define TESTS_EVENT_NAME "Global\\TestsEvent"

void compile_dll() {
    // Call the batch script to compile the DLL
    if (system(BUILD_LIB_SCRIPT) != 0) {
        std::cout << "Lib Compilation failed." << std::endl;
    } else {
        std::cout << "Lib Compilation successful." << std::endl;

        HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, HOTRELOAD_EVENT_NAME);
        if (hEvent) {
            SetEvent(hEvent);
            CloseHandle(hEvent);
            std::cout << "Signal sent to hotreload." << std::endl;
        } else {
            std::cerr << "Failed to open hotreload event (" << GetLastError() << ")" << std::endl;
        }
    }

    if (system(BUILD_TESTS_SCRIPT) != 0) {
        std::cout << "Tests Compilation failed." << std::endl;
    } else {
        std::cout << "Tests Compilation successful." << std::endl;

        HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, TESTS_EVENT_NAME);
        if (hEvent) {
            SetEvent(hEvent);
            CloseHandle(hEvent);
            std::cout << "Signal sent to tests." << std::endl;
        } else {
            std::cerr << "Failed to open tests event (" << GetLastError() << ")" << std::endl;
        }
    }
}

void watch_directory(const std::string& directory_to_watch) {
    HANDLE hDir = CreateFile(
        directory_to_watch.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateFile failed (" << GetLastError() << ")" << std::endl;
        return;
    }

    char buffer[1024];
    DWORD bytesReturned;
    OVERLAPPED overlapped = {};

    while (true) {
        if (ReadDirectoryChangesW(
            hDir,
            buffer,
            sizeof(buffer),
            TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE,
            &bytesReturned,
            &overlapped,
            NULL
        )) {
            WaitForSingleObject(overlapped.hEvent, INFINITE);
            FILE_NOTIFY_INFORMATION* pNotify;
            int offset = 0;
            do {
                pNotify = (FILE_NOTIFY_INFORMATION*)((char*)buffer + offset);
                std::wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
                std::wcout << L"Change detected in: " << fileName << std::endl;
                offset += pNotify->NextEntryOffset;
            } while (pNotify->NextEntryOffset);

            compile_dll();
        }
    }

    CloseHandle(hDir);
}

int main() {
    const std::string directories_to_watch[] = { "../src", "../tests" };

    for (const auto& dir : directories_to_watch) {
        std::thread watcher(watch_directory, dir);
        watcher.detach();
    }

    // Keep the main thread alive indefinitely
    while (true) {
        Sleep(1000);
    }

    return 0;
}
