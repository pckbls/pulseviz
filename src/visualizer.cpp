#include "visualizer.h"
#include "visualizer/dummy.h"
#include "visualizer/waveform.h"
#include "visualizer/spectrum.h"
#include "visualizer/octavebands.h"
#include "visualizer/spectrogram.h"
#include "visualizer/spectrogram3d.h"

// Names cannot be directly inserted into the vector at this point
// since the strings might be uninitialized [...]
static std::vector<std::string> visualizer_order;

void loadConfig(const IniParser& ini)
{
    // [...] instead we do this here.
    visualizer_order.push_back(DummyVisualizerNew::name);
    visualizer_order.push_back(WaveFormVisualizer::name);
    visualizer_order.push_back(SpectrumVisualizer::name);
    visualizer_order.push_back(OctavebandsVisualizer::name);
    visualizer_order.push_back(SpectrogramVisualizer::name);
    visualizer_order.push_back(Spectrogram3DVisualizer::name);

    WaveFormVisualizer::loadConfig(ini);
    SpectrumVisualizer::loadConfig(ini);
    OctavebandsVisualizer::loadConfig(ini);
    SpectrogramVisualizer::loadConfig(ini);
    Spectrogram3DVisualizer::loadConfig(ini);
}

std::string getNextVisualizerName(const std::string& name)
{
    for (auto it = visualizer_order.begin(); it != visualizer_order.end(); it++)
    {
        if (name == *it)
        {
            it++;
            if (it == visualizer_order.end())
                it = visualizer_order.begin();

            return *it;
        }
    }

    throw "This should never happen at all.";
}

std::unique_ptr<Visualizer> createVisualizer(const std::string& name)
{
    Visualizer* result = nullptr;

    if (name == DummyVisualizerNew::name)
        result = new DummyVisualizerNew();
    else if (name == WaveFormVisualizer::name)
        result = new WaveFormVisualizer();
    else if (name == SpectrumVisualizer::name)
        result = new SpectrumVisualizer();
    else if (name == OctavebandsVisualizer::name)
        result = new OctavebandsVisualizer();
    else if (name == SpectrogramVisualizer::name)
        result = new SpectrogramVisualizer();
    else if (name == Spectrogram3DVisualizer::name)
        result = new Spectrogram3DVisualizer();
    else
        throw "This should never happen at all.";

    return std::unique_ptr<Visualizer>(result);
}

Visualizer::Visualizer()
{}

Visualizer::~Visualizer()
{}

void Visualizer::draw()
{}

void Visualizer::resize(int width, int height)
{
    // TODO: Remove this?
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
