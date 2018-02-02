#include <iostream>
#include <GL/glew.h>
#include "visualizer.h"

Visualizer::Visualizer(std::shared_ptr<SimpleRecordClient> src_ptr)
    :
    src(src_ptr),
    quit_thread(false)
{}

Visualizer::~Visualizer()
{}

void Visualizer::startThread()
{
    this->audio_thread = std::thread(&Visualizer::audioThreadFunc, this);
}

void Visualizer::stopThread()
{
    this->quit_thread = true;
    if (this->audio_thread.joinable())
        this->audio_thread.join();
}

void Visualizer::audioThreadFunc()
{
    while (!this->quit_thread)
    {
        std::vector<float> chunk(512);
        this->src->read(chunk);
    }
}

void Visualizer::onFramebuffersizeChanged(unsigned int /* width */, unsigned int /* height */)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1.0, -1.0, 1.0, -1, 1);
}

void Visualizer::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0.0, -1.0);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(1.0, -1.0);
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex2f(0.5, 1.0);
    glEnd();
}

const char* Visualizer::getTitle()
{
    return "Visualizer base";
}

void Visualizer::setColorScheme(ColorScheme &colorscheme)
{
    this->colorscheme = std::make_shared<ColorScheme>(colorscheme);
}
