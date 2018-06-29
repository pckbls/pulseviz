#pragma once

#include "visualizer.h"
#include "gfx/window.h"

class PulseViz;

class VisualizerWindow : public Window
{
    friend class PulseViz;

public:
    VisualizerWindow(PulseViz& pulseviz);
    VisualizerWindow(const VisualizerWindow&) = delete;
    ~VisualizerWindow() override;

    void attachVisualizer(std::shared_ptr<Visualizer> visualizer);
    void detatchVisualizer();
    bool isVisualizerAttached();
    Visualizer& getVisualizer();

protected:
    std::shared_ptr<Visualizer> visualizer;
    PulseViz& pulseviz;
};
