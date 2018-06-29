#include "visualizerwindow.h"

VisualizerWindow::VisualizerWindow(PulseViz& pulseviz)
    :
    pulseviz(pulseviz)
{}

VisualizerWindow::~VisualizerWindow()
{
    // TODO: Handle exception thrown below?
    this->detatchVisualizer();
}

void VisualizerWindow::attachVisualizer(std::shared_ptr<Visualizer> visualizer)
{
    if (this->visualizer)
        this->detatchVisualizer();

    this->visualizer = visualizer;

    int width, height;
    glfwGetWindowSize(this->window_handle, &width, &height);
    this->visualizer->resize(width, height);

    std::string window_title = "pulseviz - " + visualizer->getTitle();
    glfwSetWindowTitle(this->window_handle, window_title.c_str());

    this->visualizer->attachSRC();
}

void VisualizerWindow::detatchVisualizer()
{
    this->visualizer->detatchSRC();
    // TODO: Throw exception if now visualizer has been attached?
    this->visualizer.reset(); // TODO: Is reset correct?
}

bool VisualizerWindow::isVisualizerAttached()
{
    return this->visualizer ? true : false; // TODO
}

Visualizer& VisualizerWindow::getVisualizer()
{
    return *this->visualizer.get();
}
