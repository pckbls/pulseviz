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
    virtual const std::string getTitle() const = 0;
    virtual void draw();
    virtual void resize(int width, int height);
};

class VisualizerFactory
{
public:
    VisualizerFactory();
    virtual ~VisualizerFactory();
    virtual std::unique_ptr<Visualizer> create() const = 0;
};
