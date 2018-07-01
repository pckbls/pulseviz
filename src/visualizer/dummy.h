#pragma once

#include "../visualizer.h"

class DummyVisualizerNew : public Visualizer
{
public:
    static const std::string name;
    static const std::string title;

    const std::string& getName() const;
    const std::string& getTitle() const;

    void draw();
};

class DummyVisualizerFactory : public VisualizerFactory
{
    std::unique_ptr<Visualizer> create() const;
};
