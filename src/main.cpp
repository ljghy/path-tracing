#include "path_tracer/Renderer.h"
#include "path_tracer/Scene.h"
#include <iostream>
#include <string>

class Application
{
public:
    void run(int argc, char *argv[])
    {
        loadRes(argc, argv);
        render();
        output();
    }

private:
    Scene scene;
    Renderer renderer;

    void loadRes(int argc, char *argv[])
    {
        std::string scenePath, rendererPath;
        if (argc == 1)
        {
            std::cout << "Input scene file path: \n";
            std::cin >> scenePath;
            ++argc;
        }
        else
            scenePath = argv[1];
        if (argc == 2)
        {
            std::cout << "Input renderer file path: \n";
            std::cin >> rendererPath;
        }
        else
            rendererPath = argv[2];

        printf("Loading scene...\n");
        scene.loadScene(scenePath);
        printf("Loading render configuration...\n");
        renderer.loadConfig(rendererPath);
    }

    void render()
    {
        renderer.render(&scene);
    }

    void output()
    {
        renderer.outputPNG();
    }
};

int main(int argc, char *argv[])
{
    try
    {
        Application app;
        app.run(argc, argv);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}