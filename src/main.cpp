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
        },
        {
            "numix",
            {
                {68.0/255.0, 68.0/255.0, 68.0/255.0},
                //{51.0/255.0, 51.0/255.0, 51.0/255.0},
                {
                    {0.00, {240.0f/255.0f, 84.0f/255.0f, 76.0f/255.0f}},
                    {1.00, {240.0f/255.0f, 84.0f/255.0f, 76.0f/255.0f}}
                }
            },
        },
        {
            "numix2",
            {
                {68.0/255.0, 68.0/255.0, 68.0/255.0},
                {
                    {0.00, {68.0/255.0, 68.0/255.0, 68.0/255.0}},
                    {0.10, {240.0f/255.0f, 84.0f/255.0f, 76.0f/255.0f}},
                    {1.00, {240.0f/255.0f, 84.0f/255.0f, 76.0f/255.0f}}
                }
            }
        },
    };

    return result;

#if 0
    palettes = {
        {
            "grayscale",
            Palette(
                {0.0, 0.0, 0.0},
                PaletteTexture(palette_width, {
                    {0.00, {0.0, 0.0, 0.0}},
                    {1.00, {1.0, 1.0, 1.0}}
                })
            )
        },
        {
            // TODO: https://stackoverflow.com/questions/28495390/thermal-imaging-palette
            "ironbow",
            Palette(
                {0.0, 0.0, 0.0},
                PaletteTexture(palette_width, {
                    {0.00, {0.0f, 0.0f, 0.0f}},
                    {0.10, {0.0f, 0.0f, 0.5f}},
                    {0.30, {1.0f, 0.0f, 1.0f}},
                    {0.80, {1.0f, 1.0f, 0.0f}},
                    {1.00, {1.0f, 1.0f, 1.0f}}
                })
            )
        },
        {
            "bluered",
            Palette(
                {0.0, 0.0, 0.0},
                PaletteTexture(palette_width, {
                    {0.00, {0.0f, 0.0f, 0.1f}},
                    {0.15, {0.0f, 0.5f, 1.0f}},
                    {0.45, {1.0f, 1.0f, 1.0f}},
                    {0.65, {1.0f, 1.0f, 0.0f}},
                    {1.00, {1.0f, 1.0f, 0.0f}}
                })
            )
        },
        {
            "solarized",
            Palette(
                {0.99f, 0.96f, 0.89f},
                PaletteTexture(palette_width, {
                    {0.00, {211.0f/255.0f, 54.0f/255.0f, 130.0f/255.0f}},
                    {1.00, {220.0f/255.0f, 50.0f/255.0f, 47.0f/255.0f}}
                })
            )
        }
    };
#endif
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
