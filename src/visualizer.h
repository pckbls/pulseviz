#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include "pulseaudio.h"
#include "colorscheme.h"

class VisualizerWindow;

class Visualizer
{
    friend class VisualizerWindow;

public:
    Visualizer();
    virtual ~Visualizer();
    virtual const char *getTitle();
    virtual void render();
    virtual void onFramebuffersizeChanged(unsigned int width, unsigned int height);
    virtual void setColorScheme(ColorScheme& colorscheme);

protected:
    virtual void audioThreadFunc();
    void startThread();
    void stopThread();

    SimpleRecordClient src;
    std::shared_ptr<ColorScheme> colorscheme; // TODO: Why shared_ptr?
    std::atomic_bool quit_thread;
    std::thread audio_thread;
};

#endif // VISUALIZER_H
