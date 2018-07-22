#include <iostream>
#include <GL/glew.h>
#include "window.h"

// Taken from: https://www.khronos.org/opengl/wiki/OpenGL_Error#Catching_errors_.28the_easy_way.29
void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam)
{
    (void)source;
    (void)id;
    (void)length;
    (void)userParam;

    fprintf(
        stderr,
        "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message
    );
}

Window::Window()
    :
    window_handle(nullptr)
{
    this->window_handle = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!this->window_handle)
        throw "Cannot create window";

    glfwSetWindowUserPointer(this->window_handle, this);

    glfwMakeContextCurrent(this->window_handle);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << glewGetErrorString(err) << std::endl;
        throw "Cannot initialize GLEW";
    }
    std::cout << "OpenGL version supported by this platform: " << glGetString(GL_VERSION) << std::endl;

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, nullptr);
#endif
}

Window::~Window()
{
    if (this->window_handle)
        glfwDestroyWindow(this->window_handle);
}

GLFWwindow* Window::getHandle() const
{
    return this->window_handle;
}
