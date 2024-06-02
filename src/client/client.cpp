#include "client.h"
#include <iostream>
#include <string>


 
#include "linmath.h"

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define PORT 8080
#define BUFFER_SIZE 1024

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};  

static const char* vertex_shader_text =
"#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

static const char* fragment_shader_text =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
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

    glGenVertexArrays(1, &(clientData->VAO));  
    glBindVertexArray(clientData->VAO);

    glGenBuffers(1, &(clientData->vertex_buffer));
    glBindBuffer(GL_ARRAY_BUFFER, clientData->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
    clientData->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(clientData->vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(clientData->vertex_shader);
    GLint success;
    glGetShaderiv(clientData->vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(clientData->vertex_shader, sizeof(infoLog), NULL, infoLog);
        std::cerr << "Vertex Shader Compilation Failed. Info Log: " << infoLog << std::endl;
        return false;
    }
 
    clientData->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(clientData->fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(clientData->fragment_shader);
    glGetShaderiv(clientData->fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(clientData->fragment_shader, sizeof(infoLog), NULL, infoLog);
        std::cerr << "Fragment Shader Compilation Failed. Info Log: " << infoLog << std::endl;
        return false;
    }
 
    clientData->program = glCreateProgram();
    glAttachShader(clientData->program, clientData->vertex_shader);
    glAttachShader(clientData->program, clientData->fragment_shader);
    glLinkProgram(clientData->program);
    glGetProgramiv(clientData->program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(clientData->program, sizeof(infoLog), NULL, infoLog);
        std::cerr << "Program Linking Failed. Info Log: " << infoLog << std::endl;
        return false;
    }


    glUseProgram(clientData->program);

    clientData->mvp_location = glGetUniformLocation(clientData->program, "MVP");
    clientData->vpos_location = glGetAttribLocation(clientData->program, "vPos");
    clientData->vcol_location = glGetAttribLocation(clientData->program, "vCol");
 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 
   
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set a background color
    return true;
}

void client_update(ClientData* clientData, void* registry)
{
    float ratio;
    int width, height;
    mat4x4 m, p, mvp;

    glfwGetFramebufferSize(clientData->window, &width, &height);
    ratio = width / (float) height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    mat4x4_identity(m);
    mat4x4_rotate_Z(m, m, (float) glfwGetTime());
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);

    glUseProgram(clientData->program);
    glBindVertexArray(clientData->VAO);
    glUniformMatrix4fv(clientData->mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(clientData->window);
    glfwPollEvents();
}

void client_shutdown(ClientData* clientData)
{
    glfwDestroyWindow(clientData->window);
    glfwTerminate();
}
