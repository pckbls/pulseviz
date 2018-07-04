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
    // Set up a parallel projection.
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1.0, -1.0, 1.0, -1, 1);
}

void Visualizer::attachSRC()
{}

void Visualizer::detatchSRC()
{}

VisualizerFactory::VisualizerFactory() {}
VisualizerFactory::~VisualizerFactory() {}
