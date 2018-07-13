#include <list>
#include <istream>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "pulseviz.h"
#include "pulseaudio.h"
#include "util.h"
#include "visualizer.h"
#include "visualizer/dummy.h"
#include "visualizer/waveform.h"
#include "visualizer/spectrum.h"
#include "visualizer/octavebands.h"
#include "visualizer/spectrogram.h"
#include "visualizer/spectrogram3d.h"

PulseViz::PulseViz(int argc, char** argv)
{
    for (int i = 0; i < argc; i++)
        this->args.push_back(argv[i]);

    this->binary_path = util::dirname(this->args[0]);
}

int PulseViz::run()
{
    if (this->args.size() == 2 && this->args[1] == "-h")
    {
        this->printHelp();
        return 0;
    }
    else if (this->args.size() != 1)
    {
        std::cerr << "Unknown argument(s)" << std::endl;
        return -1;
    }

    if (!this->loadConfig())
    {
        std::cerr << "Could not load configuration." << std::endl;
        return -1;
    }

    if (!this->loadShaders())
    {
        std::cerr << "Shader initialization failed." << std::endl;
        return -1;
    }

    // TODO: Catch and print caught exceptions.
    if (!this->manageWindows())
    {
        std::cerr << "Something very bad happened." << std::endl;
        return -1;
    };

    return 0;
}

bool PulseViz::loadConfig()
{
    std::string home_directory = util::getHomeDirectory();
    std::vector<std::string> paths = {
        this->binary_path + "/pulseviz.ini",
        home_directory + "/.pulseviz/pulseviz.ini"
    };

    // Try to open the INI file.
    bool file_opened = false;
    std::ifstream stream;
    for (const auto& path: paths)
    {
        stream.open(path);
        if (stream.is_open())
        {
            std::cout << "Reading configuration values from: " << path << std::endl;
            file_opened = true;
            break;
        }
    }
    if (!file_opened)
    {
        std::cerr << "Tried to open the following configuration files but cannot read either of them:" << std::endl;
        for (const auto& path: paths)
            std::cerr << "  * " << path << std::endl;
        return false;
    }

    // Attempt to parse it.
    IniParser ini;
    try
    {
        ini.parse(stream);
    }
    catch (IniParserException& e)
    {
        std::cerr << "Cannot parse configuration file." << std::endl;
        std::cerr << "Reason: " << e.what() << std::endl;
        std::cerr << "Line " << e.getLine() << ": " << e.getLineContent() << std::endl;
        return false;
    }

    stream.close();

    // Now let all available visualizers have a look at our parsed INI file
    // and make them validate their matching sections.
    try
    {
        this->passConfigToVisualizers(ini);
    }
    catch (IniParserTypeConversionException& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    catch (IniParserDataException& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    catch (const char *c)
    {
        std::cerr << c << std::endl;
        return false;
    }

    return true;
}

void PulseViz::passConfigToVisualizers(IniParser& ini)
{
    this->visualizer_factories.emplace_back(new DummyVisualizerFactory());
    this->visualizer_factories.emplace_back(new WaveFormVisualizerFactory(ini));
    this->visualizer_factories.emplace_back(new SpectrumVisualizerFactory(ini));
    this->visualizer_factories.emplace_back(new OctavebandsVisualizerFactory(ini));
    this->visualizer_factories.emplace_back(new SpectrogramVisualizerFactory(ini));
    this->visualizer_factories.emplace_back(new Spectrogram3DVisualizerFactory(ini));
}

bool PulseViz::loadShaders()
{
    std::vector<std::string> shader_directories = {
        this->binary_path + "/shader/"
    };

    std::string& path = shader_directories[0];
    if (!util::dir_exists(path))
    {
        std::cerr << "'" << path << "' is not a directory or cannot be opened." << std::endl;
        return false;
    }

    std::cout << "Using shader sources found in: " << path << std::endl;
    return true;
}

void PulseViz::printHelp()
{
    std::cout << "pulseviz 0.0.0" << std::endl; // TODO: Update version!
    std::cout << "  Author: Patrick Bartels <pckbls>" << std::endl;
    std::cout << "  GitHub: https://gitlab.com/pckbls/pulseviz" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  pulseviz" << std::endl;
    std::cout << std::endl;
}

bool PulseViz::manageWindows()
{
    glfwInit();

    this->createWindow();

    // TODO: Handle errors!
    while (this->windows.size() > 0)
    {
        auto it = this->windows.begin();
        while (it != this->windows.end())
        {
            VisualizerWindow& window = **it; // TODO: Looks funny.
            if (glfwWindowShouldClose(window.getHandle()))
                it = this->windows.erase(it); // TODO: Correct?
            else
            {
                if (window.isVisualizerAttached())
                {
                    glfwMakeContextCurrent(window.getHandle());
                    window.getVisualizer().draw();
                }
                glfwSwapBuffers(window.getHandle());
                it++;
            }
        }

        glfwPollEvents();
    }

    glfwTerminate();

    return true;
}

void PulseViz::createWindow()
{
    std::unique_ptr<VisualizerWindow> window(new VisualizerWindow(*this, this->visualizer_factories));

    glfwSetFramebufferSizeCallback(window->getHandle(), PulseViz::onFramebufferSizeChange);
    glfwSetKeyCallback(window->getHandle(), PulseViz::onKey);

    // TODO: Do we need this?
    glfwMakeContextCurrent(window->getHandle());

    window->nextVisualizer();

    this->windows.push_back(std::move(window)); // TODO: Really std::move?
}

void PulseViz::nextVisualizer(VisualizerWindow* window)
{
    window->nextVisualizer();
}

void PulseViz::onFramebufferSizeChange(GLFWwindow* window, int width, int height)
{
    // TODO: Really static_cast?
    VisualizerWindow* visualizer_window = static_cast<VisualizerWindow*>(glfwGetWindowUserPointer(window));
    if (visualizer_window->isVisualizerAttached())
    {
        glfwMakeContextCurrent(visualizer_window->getHandle()); // TODO: Very important!
        visualizer_window->getVisualizer().resize(width, height);
    }
}

void PulseViz::onKey(GLFWwindow* window, int key, int /* scancode */, int action, int mods)
{
    // We only want to handle the release of keys with no modifier (Ctrl, Alt ...) being held.
    if (action != GLFW_RELEASE || mods != 0)
        return;

    VisualizerWindow* visualizer_window = static_cast<VisualizerWindow*>(glfwGetWindowUserPointer(window));

    switch (key)
    {
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(window, 1);
        break;
        case GLFW_KEY_C:
            visualizer_window->pulseviz.createWindow();
        break;
        case GLFW_KEY_SPACE:
            visualizer_window->pulseviz.nextVisualizer(visualizer_window);
        break;
    }
}
