#pragma once

#include <vector>
#include <map>
#include <memory>
#include <string>
#include "pulseaudio.h"
#include "iniparser.h"

// TODO: Read: Factory Pattern in C++ - CodeProject
// https://www.codeproject.com/Articles/363338/Factory-Pattern-in-Cplusplus

class Visualizer
{
public:
    Visualizer();
    Visualizer(const Visualizer&) = delete;
    virtual ~Visualizer();

    // TODO: Remove!
    virtual const std::string& getName() const = 0;

    virtual void attachSRC();
    virtual void detatchSRC();

    virtual const std::string& getTitle() const = 0;
    virtual void draw();
    virtual void resize(int width, int height);
};

// TODO: Put those into a name space!
std::unique_ptr<Visualizer> createVisualizer(const std::string& name);
void loadConfig(const IniParser& ini); // TODO: Find a better name! This not only initializes the visualizers but also the "factory".
std::string getNextVisualizerName(const std::string& name);
