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

static std::map<std::string, std::unique_ptr<VisualizerFactory>> factories;

void loadConfig(const IniParser& ini)
{
    // [...] instead we do this here.
    visualizer_order.push_back(DummyVisualizerNew::name);
    visualizer_order.push_back(WaveFormVisualizer::name);
    visualizer_order.push_back(SpectrumVisualizer::name);
    visualizer_order.push_back(OctavebandsVisualizer::name);
    visualizer_order.push_back(SpectrogramVisualizer::name);
    visualizer_order.push_back(Spectrogram3DVisualizer::name);

    factories[DummyVisualizerNew::name] = std::unique_ptr<VisualizerFactory>(new DummyVisualizerFactory());
    factories[WaveFormVisualizer::name] = std::unique_ptr<VisualizerFactory>(new WaveFormVisualizerFactory(ini));
    factories[SpectrumVisualizer::name] = std::unique_ptr<VisualizerFactory>(new SpectrumVisualizerFactory(ini));
    factories[OctavebandsVisualizer::name] = std::unique_ptr<VisualizerFactory>(new OctavebandsVisualizerFactory(ini));
    factories[SpectrogramVisualizer::name] = std::unique_ptr<VisualizerFactory>(new SpectrogramVisualizerFactory(ini));
    factories[Spectrogram3DVisualizer::name] = std::unique_ptr<VisualizerFactory>(new Spectrogram3DVisualizerFactory(ini));
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
    return factories.at(name)->create();
}

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
