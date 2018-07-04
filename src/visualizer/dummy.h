#pragma once

#include "../visualizer.h"

class DummyVisualizerNew : public Visualizer
{
public:
    const std::string getTitle() const override;
    void draw() override;
};

class DummyVisualizerFactory : public VisualizerFactory
{
    std::unique_ptr<Visualizer> create() const;
};
