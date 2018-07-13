#pragma once

#include "../visualizer.h"
#include "../gfx/palette.h"
#include "../gfx/shader.h"

class DummyVisualizerNew : public Visualizer
{
public:
    DummyVisualizerNew();

    const std::string getTitle() const override;
    void resize(int width, int height) override;
    void draw() override;

private:
    PaletteTexture palette_a, palette_b;
    Shader shader;
};

class DummyVisualizerFactory : public VisualizerFactory
{
    std::unique_ptr<Visualizer> create() const;
};
