#include "hotreload.h"
#include <iostream>

static HINSTANCE hLib = nullptr;

bool loadLibrary(const std::string& libName) {
    hLib = LoadLibraryA(libName.c_str());
    if (!hLib) {
        std::cerr << "Library couldn't be loaded: " << libName << " (Error: " << GetLastError() << ")\n";
        return false;
    }
    return true;
}

void unloadLibrary() {
    if (hLib) {
        FreeLibrary(hLib);
        hLib = nullptr;
    }
}

InitFunc getInitFunction() {
    return (InitFunc)GetProcAddress(hLib, "client_init");
}

ReInitFunc getReInitFunction() {
    return (InitFunc)GetProcAddress(hLib, "client_re_init");
}

UpdateFunc getUpdateFunction() {
    return (UpdateFunc)GetProcAddress(hLib, "client_update");
}

ShutdownFunc getShutdownFunction() {
    return (ShutdownFunc)GetProcAddress(hLib, "client_shutdown");
}

std::string getCurrentWorkingDirectory() {
    char* buffer;
    if ((buffer = getcwd(NULL, 0)) == NULL) {
        perror("_getcwd error");
        return "";
    } else {
        std::string cwd(buffer);
        free(buffer);
        return cwd;
    }
}