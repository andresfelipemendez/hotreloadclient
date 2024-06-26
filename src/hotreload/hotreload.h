#ifndef HOT_RELOAD_H
#define HOT_RELOAD_H

#include <string>

typedef bool (*InitFunc)(struct ClientData*, void*);
typedef bool (*ReInitFunc)(struct ClientData*, void*);
typedef void (*UpdateFunc)(struct ClientData*, void*);
typedef void (*ShutdownFunc)(struct ClientData*);

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#define getcwd _getcwd
#define PATH_MAX MAX_PATH
#else
#include <unistd.h>
#include <dlfcn.h>
#endif

bool loadLibrary(const std::string& libName);
void unloadLibrary();
InitFunc getInitFunction();
ReInitFunc getReInitFunction();
UpdateFunc getUpdateFunction();
ShutdownFunc getShutdownFunction();
std::string getCurrentWorkingDirectory();

#endif // HOT_RELOAD_H