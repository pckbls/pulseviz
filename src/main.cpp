#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "visualizerwindow.h"

std::map<std::string, ColorScheme> initializeColorSchemes()
{
    std::map<std::string, ColorScheme> result = {
        {
            "grayscale",
            {
                {0.0, 0.0, 0.0},
                {
                    {0.00, {0.0, 0.0, 0.0}},
                    {0.75, {0.9, 0.9, 0.9}},
                    {1.0, {1.0, 1.0, 1.0}},
                }
            },
        },
        {
            "ironbow",
            {
                {0.0, 0.0, 0.0},
                {
                    {0.00, {0.0f, 0.0f, 0.0f}},
                    {0.10, {0.0f, 0.0f, 0.5f}},
                    {0.30, {1.0f, 0.0f, 1.0f}},
                    {0.80, {1.0f, 1.0f, 0.0f}},
                    {1.00, {1.0f, 1.0f, 1.0f}}
                }
            }
        }
    };

    return result;
}

int main(int /* argc */, char** /* argv */)
{
    int return_value = 0;

    if (glfwInit())
    {
        try
        {
            VisualizerWindow window(initializeColorSchemes());

            while (!window.shouldClose())
            {
                window.render();

                window.swapBuffers();
                glfwPollEvents();
            }
        }
        catch (const char* e)
        {
            std::cerr << "Caught exception: " << e << std::endl;
            return_value = -1;
        }

        glfwTerminate();
    }
    else
    {
        std::cerr << "Cannot initialize GLFW." << std::endl;
        return_value = -1;
    }

    return return_value;
}
