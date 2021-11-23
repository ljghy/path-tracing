#include "path_tracer/Renderer.h"
#include "path_tracer/Scene.h"

#include <iostream>
#include <string>

int main(int argc, char *argv[])
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
    Scene scene;
    if (!scene.loadScene(scenePath))
    {
        printf("Failed to load scene.\n");
        return 0;
    }

    printf("Loading render configuration...\n");
    Renderer renderer;
    if (!renderer.loadConfig(rendererPath))
    {
        printf("Failed to load render configuration.\n");
        return 0;
    }

    renderer.render(&scene);
    renderer.outputPNG();

    return 0;
}