#ifndef WINDOW_H
#define WINDOW_H

#include <memory>
#include <map>
#include <GLFW/glfw3.h>

class Window
{
public:
    Window();
    Window(const Window&) = delete;
    virtual ~Window();

    GLFWwindow* getHandle() const;

protected:
    GLFWwindow* window_handle; // TODO: Can we transform this into a smart ptr?
};

#endif // WINDOW_H
