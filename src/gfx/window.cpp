#include <map>
#include <iostream>
#include <GL/glew.h>
#include "window.h"

// Unfortunately this does not work, see:
// http://bannalia.blogspot.de/2016/07/passing-capturing-c-lambda-functions-as.html
// TODO: We probably do not need the map, see:
// http://www.glfw.org/docs/3.0/group__window.html#ga3d2fc6026e690ab31a13f78bc9fd3651
static std::map<GLFWwindow*, Window*> visualizer_windows;

static void _onFramebufferSizeChange(GLFWwindow* window, int width, int height)
{
    auto visualizer_window = visualizer_windows.at(window);
    visualizer_window->onFramebufferSizeChange(width, height);
}

static void _onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto visualizer_window = visualizer_windows.at(window);
    visualizer_window->onKeyPress(key, scancode, action, mods);
}

Window::Window()
    :
    window_handle(0)
{
    this->window_handle = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!this->window_handle)
        throw "Cannot create window";

    visualizer_windows[this->window_handle] = this;

    glfwMakeContextCurrent(this->window_handle);

    glfwSetFramebufferSizeCallback(this->window_handle, _onFramebufferSizeChange);
    glfwSetKeyCallback(this->window_handle, _onKeyPress);

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
    auto it = visualizer_windows.find(this->window_handle);
    visualizer_windows.erase(it);

    if (this->window_handle)
        glfwDestroyWindow(this->window_handle);
}

void Window::onFramebufferSizeChange(int /* width */, int /* height */)
{}

void Window::onKeyPress(int /* key */, int /* scancode */, int /* action */, int /* mods */)
{}

void Window::swapBuffers()
{
    glfwSwapBuffers(this->window_handle);
}

void Window::setShouldClose()
{
    glfwSetWindowShouldClose(this->window_handle, GLFW_TRUE);
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(this->window_handle);
}

void Window::toggleFullScreen()
{
    if (glfwGetWindowMonitor(this->window_handle) == NULL)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(this->window_handle, monitor, 0, 0,
                             mode->width, mode->height, mode->refreshRate);
    }
    else
    {
        // TODO: Old width/height
        glfwSetWindowMonitor(this->window_handle, NULL, 0, 0, 640, 480, 60);
    }
}

void Window::render()
{
    glfwMakeContextCurrent(this->window_handle);
}
