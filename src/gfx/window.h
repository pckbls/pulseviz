#ifndef WINDOW_H
#define WINDOW_H

#include <memory>
#include <GLFW/glfw3.h>

class Window
{
public:
    Window();
    virtual ~Window();
    virtual void onFramebufferSizeChange(int width, int height);
    virtual void onKeyPress(int key, int scancode, int action, int mods);
    virtual void render();
    void swapBuffers();
    void toggleFullScreen();

    // TODO: Do we really need those?
    void setShouldClose();
    bool shouldClose();

protected:
    GLFWwindow* window_handle; // TODO: Can we transform this into a smart ptr?
};

#endif // WINDOW_H
