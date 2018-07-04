#ifndef PULSEVIZ_H
#define PULSEVIZ_H

#include "iniparser.h"
#include "visualizerwindow.h"
#include <vector>
#include <GLFW/glfw3.h>

class PulseViz
{
public:
    PulseViz(int argc, char** argv);
    int run();

private:
    void printHelp();

    bool loadConfig();
    void passConfigToVisualizers(IniParser& ini);

    bool loadShaders();
    bool manageWindows();

    void createWindow();
    void nextVisualizer(VisualizerWindow* window);

    // TODO: Is the protected section right?
    static void onFramebufferSizeChange(GLFWwindow* window, int width, int height);
    static void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);

    std::vector<std::string> args;
    std::string binary_path;
    std::vector<std::unique_ptr<VisualizerWindow>> windows; // TODO: Consider using a list?
    std::vector<std::unique_ptr<VisualizerFactory>> visualizer_factories;
};

#endif // PULSEVIZ_H
