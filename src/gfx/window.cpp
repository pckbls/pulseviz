#include <iostream>
#include <GL/glew.h>
#include "window.h"

Window::Window()
    :
    window_handle(nullptr)
{
    this->window_handle = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!this->window_handle)
        throw "Cannot create window";

    glfwSetWindowUserPointer(this->window_handle, this);

    glfwMakeContextCurrent(this->window_handle);

    // TODO: Can we actually call this multiple times?
    // Does this work with multiple windows?
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << glewGetErrorString(err) << std::endl;
        throw "Cannot initialize GLEW";
    }
    std::cout << "OpenGL version supported by this platform: " << glGetString(GL_VERSION) << std::endl;

    // TODO: Check version!
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
