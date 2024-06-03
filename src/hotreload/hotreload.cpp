#include <iostream>
#include <entt/entt.hpp>
#include <string>
#include <filesystem>
#include <thread>
#include <atomic>
#include "../client/client.h"
#include "hotreload.h"

#define HOTRELOAD_EVENT_NAME "Global\\ReloadEvent"

namespace fs = std::filesystem;
std::atomic<bool> reloadFlag(false);

void waitForReloadSignal(HANDLE hEvent) {
    while (true) {
        DWORD dwWaitResult = WaitForSingleObject(hEvent, INFINITE);

        switch (dwWaitResult) {
            case WAIT_OBJECT_0:
                std::cout << "Event signaled. Reloading DLL..." << std::endl;
                reloadFlag.store(true);
                ResetEvent(hEvent);
                break;
            default:
                std::cerr << "WaitForSingleObject failed (" << GetLastError() << ")" << std::endl;
                return;
        }
    }
}

int main(int argc, char* argv[]) 
{
    HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, HOTRELOAD_EVENT_NAME);
    if (hEvent == NULL) {
        std::cerr << "CreateEvent failed (" << GetLastError() << ")" << std::endl;
        return 1;
    }

    std::string directory_to_watch_str = getCurrentWorkingDirectory();

    if (directory_to_watch_str.empty()) {
        return 1;
    }

    // Constructing paths for DLLs (or shared libraries on Linux)
    std::string originalDLLpath = directory_to_watch_str + "/build/client_lib.dll";
    std::string copyDLLpath = directory_to_watch_str + "/build/client_lib_copy.dll";

    if (!fs::exists(originalDLLpath)) {
        std::cerr << "Original DLL not found: " << originalDLLpath << '\n';
        return -1;
    } else {
        fs::copy(originalDLLpath, copyDLLpath, fs::copy_options::overwrite_existing);
    }

    ClientData clientData;
    entt::registry registry;

    if (!loadLibrary(copyDLLpath)) {
        return -1;
    }

    InitFunc init = getInitFunction();
    UpdateFunc update = getUpdateFunction();
    ShutdownFunc shutdown = getShutdownFunction();
    ReInitFunc reinit = getReInitFunction();

    if (!init || !update || !shutdown) {
        std::cerr << "Unable to find necessary functions in the library" << std::endl;
        unloadLibrary();
        return -1;
    }

    if (!init(&clientData, &registry)){
        std::cerr << "Unable to initialize the client library" << std::endl;
    }

    std::thread signalThread(waitForReloadSignal, hEvent);
    while (true) 
    {
        if (reloadFlag.load()) 
        {
            reloadFlag.store(false);
            
            std::cout << "Reload DLL" << std::endl;

            unloadLibrary();
            
            std::cout << "unloadLibrary DLL" << std::endl;
            fs::copy(originalDLLpath, copyDLLpath, fs::copy_options::overwrite_existing);

            if (!loadLibrary(copyDLLpath)) {
                std::cerr << "Failed to reload the library" << std::endl;
                return -1;
            }
            
            std::cout << "loadLibrary DLL" << std::endl;
            init = getInitFunction();
            update = getUpdateFunction();
            shutdown = getShutdownFunction();
            reinit = getReInitFunction();

            if (!init || !update || !shutdown) {
                std::cerr << "Unable to find necessary functions in the reloaded library" << std::endl;
                unloadLibrary();
                return -1;
            }

            if (!reinit(&clientData, &registry)) {
                 std::cerr << "Unable to initialize the reloaded client library" << std::endl;
            }
        }

        update(&clientData, &registry);   
    }

    shutdown(&clientData);
    CloseHandle(hEvent);

    return 0;
}