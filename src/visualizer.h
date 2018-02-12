#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include "pulseaudio.h"
#include "colorscheme.h"

class VisualizerWindow;

class Visualizer
{
    friend class VisualizerWindow;

public:
    Visualizer();
    virtual ~Visualizer();
    virtual const char *getTitle(); // TODO: std::string
    virtual void render();
    virtual void onFramebuffersizeChanged(unsigned int width, unsigned int height);
    virtual void setColorScheme(ColorScheme& colorscheme);

protected:
    virtual void audioThreadFunc();
    void startThread();
    void stopThread();
    std::chrono::steady_clock::duration durationSinceLastUpdate();

    SimpleRecordClient src;
    std::shared_ptr<ColorScheme> colorscheme; // TODO: Why shared_ptr?
    std::atomic_bool quit_thread;
    std::thread audio_thread;
    std::chrono::steady_clock::time_point last_render_tp;
};

#endif // VISUALIZER_H
