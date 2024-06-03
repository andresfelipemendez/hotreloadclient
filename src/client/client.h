#pragma once
#include "../hotreload/hotreload.h"
#include <entt/entt.hpp>

extern "C" {
    __declspec(dllexport) bool client_init(ClientData* clientData, void* registry);
    
    __declspec(dllexport) bool client_re_init(ClientData* clientData, void* registry);
    
    __declspec(dllexport) void client_update(ClientData* clientData, void* registry);

    __declspec(dllexport) void client_shutdown(ClientData* clientData);
}

struct ClientData{
    struct GLFWwindow* window;
    unsigned int vertex_buffer, vertex_shader, fragment_shader, program,VAO;
    int mvp_location, vpos_location = 0, vcol_location;
    bool initialized = false;
};