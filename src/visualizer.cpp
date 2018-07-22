#include "visualizer.h"
#include <GL/glew.h>

Visualizer::Visualizer()
{}

Visualizer::~Visualizer()
{}

void Visualizer::draw()
{}

void Visualizer::resize(int width, int height)
{
    (void)width;
    (void)height;
}

VisualizerFactory::VisualizerFactory() {}
VisualizerFactory::~VisualizerFactory() {}
