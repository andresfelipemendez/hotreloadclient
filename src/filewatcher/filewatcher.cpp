#include <windows.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <thread>
#define BUILD_LIB_SCRIPT "buildscripts\\build_client_lib.bat"
#define BUILD_TESTS_SCRIPT "buildscripts\\build_tests.bat"
#define HOTRELOAD_EVENT_NAME "Global\\ReloadEvent"
#define TESTS_EVENT_NAME "Global\\TestsEvent"

HANDLE get_or_create_event(const std::string& event_name) {
    HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, event_name.c_str());
    if (!hEvent) {
        hEvent = CreateEvent(NULL, TRUE, FALSE, event_name.c_str());
        if (!hEvent) {
            std::cerr << "Failed to create event " << event_name << " (" << GetLastError() << ")" << std::endl;
        }
    }
    return hEvent;
}

void compile_dll() {
    HANDLE hHotReloadEvent = get_or_create_event(HOTRELOAD_EVENT_NAME);
    HANDLE hTestsEvent = get_or_create_event(TESTS_EVENT_NAME);

    // Call the batch script to compile the DLL
    if (system(BUILD_LIB_SCRIPT) != 0) {
        std::cout << "Lib Compilation failed." << std::endl;
    } else {
        std::cout << "Lib Compilation successful." << std::endl;

        if (hHotReloadEvent) {
            SetEvent(hHotReloadEvent);
            std::cout << "Signal sent to hotreload." << std::endl;
        } else {
            std::cerr << "Failed to signal hotreload event" << std::endl;
        }
    }

    if (system(BUILD_TESTS_SCRIPT) != 0) {
        std::cout << "Tests Compilation failed." << std::endl;
    } else {
        std::cout << "Tests Compilation successful." << std::endl;

        // Run tests.exe after successful compilation
        if (system("build\\tests.exe") != 0) {
            std::cout << "Tests execution failed." << std::endl;
        } else {
            std::cout << "Tests execution successful." << std::endl;
        }
    }

    if (hHotReloadEvent) {
        CloseHandle(hHotReloadEvent);
    }

    if (hTestsEvent) {
        CloseHandle(hTestsEvent);
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
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (overlapped.hEvent == NULL) {
        std::cerr << "CreateEvent failed (" << GetLastError() << ")" << std::endl;
        CloseHandle(hDir);
        return;
    }

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
            ResetEvent(overlapped.hEvent);
        }
    }

    CloseHandle(overlapped.hEvent);
    CloseHandle(hDir);
}

int main() {
    const std::string directories_to_watch[] = { "src", "tests" };

    for (const auto& dir : directories_to_watch) {
        std::thread watcher(watch_directory, dir);
        watcher.detach();
    }

    while (true) {
        Sleep(1000);
    }

    return 0;
}
