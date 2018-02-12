#ifndef VISUALIZERWINDOW_H
#define VISUALIZERWINDOW_H

#include <memory>
#include <map>
#include "visualizer.h"
#include "gfx/window.h"
#include "pulseaudio.h"

class VisualizerWindow : public Window
{
public:
    VisualizerWindow(std::map<std::string, ColorScheme> colorschemes);
    void onFramebufferSizeChange(int width, int height);
    void onKeyPress(int key, int scancode, int action, int mods);
    void render();
    Visualizer &getVisualizer();

protected:
    void cycleThroughVisualizers();
    void cycleThroughPalettes();
    template<class VisualizerClass> void createVisualizer();

    std::unique_ptr<Visualizer> visualizer;
    unsigned int visualizer_num;

    // TODO: Rename to colorschemes!
    std::map<std::string, ColorScheme> palettes;
    std::map<std::string, ColorScheme>::iterator palette_it;
};

#endif // VISUALIZERWINDOW_H
