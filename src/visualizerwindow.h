#pragma once

#include <vector>
#include "visualizer.h"
#include "gfx/window.h"

class PulseViz;

class VisualizerWindow : public Window
{
    friend class PulseViz;

public:
    using VisualizerFactories = std::vector<std::unique_ptr<VisualizerFactory>>;

    VisualizerWindow(PulseViz& pulseviz, VisualizerFactories& factories);
    VisualizerWindow(const VisualizerWindow&) = delete;
    ~VisualizerWindow() override;

    void nextVisualizer();

    // TODO: Get rid of those!
    bool isVisualizerAttached();
    Visualizer& getVisualizer();

protected:
    std::unique_ptr<Visualizer> visualizer;
    PulseViz& pulseviz;

    VisualizerFactories& visualizer_factories;
    VisualizerFactories::iterator visualizer_factories_it;

private:
    void attachVisualizer(std::unique_ptr<Visualizer> visualizer);
    void detatchVisualizer();
};
