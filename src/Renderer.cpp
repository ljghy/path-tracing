#include "path_tracer/Renderer.h"
#include <thread>
#include <stdio.h>
#include "utils/Timer.hpp"
#include "utils/JsonParser.hpp"
#include "path_tracer/shaders/Shaders.h"

Renderer::Renderer(uint16_t w, uint16_t h, uint16_t _SSP, Camera *c, Shader *s, const std::string &p, float _s, bool _g)
    : camera(c), shader(s), SSP(_SSP), renderResult(new Bitmap(w, h)), outputPath(p), scale(_s), gamma(_g), fromFile(false) {}

void Renderer::free()
{
    if (fromFile)
    {
        delete camera;
        camera = nullptr;
        delete shader;
        shader = nullptr;
    }
    delete renderResult;
    renderResult = nullptr;
}

Renderer::~Renderer() { free(); }

void renderThread(Renderer *renderer, Scene *scene, uint8_t id, uint16_t threadNum)
{
    srand(time(0) + id);
    Ray ray;
    const uint16_t &w = renderer->renderResult->w();
    const uint16_t &h = renderer->renderResult->h();

    float progress;
    uint16_t last = 0, step = h / 10;

    for (uint16_t row = id; row < h; row += threadNum)
    {
        for (uint16_t col = 0; col < w; ++col)
        {
            renderer->renderResult->setPixel(row, col, glm::vec3(0.f, 0.f, 0.f));
            for (uint16_t k = 0; k < renderer->SSP; ++k)
            {
                renderer->camera->generateRay(ray, (col + randf()) / w, (row + randf()) / h);
                renderer->renderResult->addPixel(row, col, renderer->shader->shade(scene, ray, 0) / (float)renderer->SSP);
            }
        }

        if (row > last)
        {
            last += step;
            progress = 100.f * row / h;
            printf("Thread[%d]: %.2f%%\n", (int)id, progress);
        }
    }
}
void Renderer::render(Scene *scene)
{
    uint16_t threadNum = std::max(std::thread::hardware_concurrency(), 1u);
    std::thread *threadList = new std::thread[threadNum];

    Timer timer;
    for (uint16_t i = 0; i < threadNum; ++i)
        threadList[i] = std::thread(renderThread, this, scene, i, threadNum);

    for (uint16_t i = 0; i < threadNum; ++i)
        threadList[i].join();
    printf("Rendered in %.2fs.\n", timer.queryTime() * 1e-3);

    delete[] threadList;
}

static inline glm::vec3 arr2vec3(const JsonNode &j)
{
    return glm::vec3(j[0].num, j[1].num, j[2].num);
}

bool Renderer::loadConfig(const std::string &filename)
{
    JsonParser parser;
    parser.loadFromFile(filename.c_str());
    auto err = parser.parse();
    if (err.state != PARSE_SUCCESS)
    {
        JsonParser::PrintErrMsg(err);
        return false;
    }

    try
    {
        free();
        fromFile = true;

        auto &data = *(parser.root);
        uint16_t w = data["width"].toInt();
        uint16_t h = data["height"].toInt();
        float ratio = (float)w / h;
        delete renderResult;
        renderResult = new Bitmap(w, h);
        SSP = data["SSP"].toInt();

        auto &cam = data["camera"];
        camera = new Camera(arr2vec3(cam["pos"]), arr2vec3(cam["dir"]),
                            arr2vec3(cam["up"]), cam["fov"].num, ratio);

        auto &shd = data["shader"];
        if (shd["type"].str == "path_tracing")
        {
            if (shd.findKey("max_depth"))
                shader = new PathTracingShader(shd["max_depth"].toInt());
            else
                shader = new PathTracingShader;
        }
        else if (shd["type"].str == "mirror")
        {
            if (shd.findKey("maxDepth"))
                shader = new MirrorShader(shd["max_depth"].toInt());
            else
                shader = new MirrorShader;
        }
        else if (shd["type"].str == "normal")
        {
            shader = new NormalShader;
        }

        auto &out = data["output"];
        outputPath = out["path"].str;
        if (out.findKey("scale"))
            scale = out["scale"].num;
        if (out.findKey("gamma"))
            gamma = out["gamma"].boo;
    }
    catch (const std::exception &e)
    {
        printf("%s\n", e.what());
        return false;
    }

    return true;
}

void Renderer::outputPNG()
{
    renderResult->outputPNG(outputPath, scale, gamma);
    printf("Render result saved to \"%s\".\n", outputPath.c_str());
}