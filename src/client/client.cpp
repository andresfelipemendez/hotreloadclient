#include "client.h"
#include <iostream>
#include <string>
#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define PORT 8080
#define BUFFER_SIZE 1024


static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool client_init(ClientData* clientData, void* registry){
    std::cout << "hello from the dll!\n";

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        std::cerr << "failed to initialize glfw!\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    clientData->window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    if (!clientData->window)
    {
        std::cerr << "failed to create glfw window!\n";
    }

    glfwMakeContextCurrent(clientData->window);
     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }


    return true;
}

void client_update(ClientData* clientData, void* registry)
{
    int width, height;
    glfwGetFramebufferSize(clientData->window, &width, &height);
    glViewport(0, 0, width, height);
}

void client_shutdown(ClientData* clientData)
{
    glfwDestroyWindow(clientData->window);
    glfwTerminate();
}
