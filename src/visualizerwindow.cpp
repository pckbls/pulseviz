#include "visualizerwindow.h"

VisualizerWindow::VisualizerWindow(PulseViz& pulseviz, VisualizerFactories& factories)
    :
    visualizer(nullptr), // TODO: necessary?
    pulseviz(pulseviz),
    visualizer_factories(factories)
{
    this->visualizer_factories_it = this->visualizer_factories.begin();
}

VisualizerWindow::~VisualizerWindow()
{
    // TODO: Handle exception thrown below?
    this->detatchVisualizer();
}

void VisualizerWindow::attachVisualizer(std::unique_ptr<Visualizer> visualizer)
{
    if (this->visualizer)
        this->detatchVisualizer();

    this->visualizer = std::move(visualizer);

    int width, height;
    glfwGetWindowSize(this->window_handle, &width, &height);
    this->visualizer->resize(width, height);

    std::string window_title = "pulseviz - " + this->visualizer->getTitle();
    glfwSetWindowTitle(this->window_handle, window_title.c_str());
}

void VisualizerWindow::detatchVisualizer()
{
    // TODO: Throw exception if now visualizer has been attached?
    this->visualizer.reset(); // TODO: Is reset correct?
}

bool VisualizerWindow::isVisualizerAttached()
{
    return this->visualizer ? true : false; // TODO
}

Visualizer& VisualizerWindow::getVisualizer()
{
    return *this->visualizer;
}

void VisualizerWindow::nextVisualizer()
{
    auto next_visualizer = (*this->visualizer_factories_it)->create();
    this->attachVisualizer(std::move(next_visualizer));

    this->visualizer_factories_it++;
    if (this->visualizer_factories_it == this->visualizer_factories.end())
        this->visualizer_factories_it = this->visualizer_factories.begin();
}
