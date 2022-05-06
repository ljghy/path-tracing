#include "path_tracer/Renderer.h"
#include <thread>
#include <vector>
#include <functional>
#include "utils/Timer.hpp"
#include "path_tracer/shaders/Shaders.h"

#include "utils/JsonParser.hpp"

void Renderer::renderThread(Scene *scene, uint16_t offset, uint16_t step)
{
    srand(static_cast<unsigned int>(time(0) + offset));
    Ray ray;
    const uint16_t &w = m_pRenderResult->w();
    const uint16_t &h = m_pRenderResult->h();

    float progress;
    uint16_t last = 0, s = h / 10;
    for (uint16_t row = offset; row < h; row += step)
    {
        for (uint16_t col = 0; col < w; ++col)
        {
            m_pRenderResult->setPixel(row, col, glm::vec3(0.f, 0.f, 0.f));
            for (uint16_t k = 0; k < m_SPP; ++k)
            {
                m_pCamera->generateRay(ray, (col + randf()) / w, (row + randf()) / h);
                m_pRenderResult->addPixel(row, col, m_pShader->shade(scene, ray) / (float)m_SPP);
            }
        }

        if (row > last)
        {
            last += s;
            progress = 100.f * row / h;
            printf("Thread[%d]: %.2f%%\n", (int)offset, progress);
        }
    }
}
void Renderer::render(Scene *scene)
{
    uint16_t threadNum = std::max(std::thread::hardware_concurrency(), 1u);
    std::vector<std::thread> threadList(threadNum);

    Timer timer;

    for (uint16_t offset = 0; offset < threadNum; ++offset)
        threadList[offset] = std::thread(&Renderer::renderThread, this, scene, offset, threadNum);

    for (auto &t : threadList)
        t.join();
    printf("Rendered in %.2fs.\n", timer.queryTime() * 1e-3);
}

static inline glm::vec3 arr2vec3(const JsonNode &j)
{
    return glm::vec3(j[0].getNum(), j[1].getNum(), j[2].getNum());
}

void Renderer::loadConfig(const std::string &filename)
{
    try
    {
        JsonParser parser;
        parser.loadFromFile(filename.c_str());
        auto err = parser.parse();
        if (err.state != PARSE_SUCCESS)
        {
            err.print();
        }

        auto &data = parser.root();
        uint16_t w = data["width"].toInt();
        uint16_t h = data["height"].toInt();
        float ratio = (float)w / h;

        m_pRenderResult = std::make_shared<Bitmap>(w, h);
        m_SPP = data["SPP"].toInt();

        auto &cam = data["camera"];
        m_pCamera = std::make_shared<Camera>(arr2vec3(cam["pos"]), arr2vec3(cam["dir"]),
                                             arr2vec3(cam["up"]), cam["fov"].getNum(), ratio);

        auto &shd = data["shader"];
        if (shd["type"] == "path_tracing")
        {
            if (shd.findKey("max_depth"))
                m_pShader = std::make_shared<PathTracingShader>(shd["max_depth"].toInt());
            else
                m_pShader = std::make_shared<PathTracingShader>();
        }
        else if (shd["type"] == "normal")
        {
            m_pShader = std::make_shared<NormalShader>();
        }

        auto &out = data["output"];
        m_outputPath = out["path"].getStr();
        if (out.findKey("scale"))
            m_scale = out["scale"].getNum();
        if (out.findKey("gamma"))
            m_gamma = out["gamma"].getBool();
        if (out.findKey("bloom"))
        {
            auto &bloom = out["bloom"];
            m_bloomKer = bloom["kernel_size"].toInt();
            m_bloomOn = m_bloomKer > 1;
            m_bloomLuminanceThreshold = bloom["luminance_threshold"].getNum();
        }
        if (out.findKey("hdr"))
        {
            m_hdrExposure = out["hdr"].getNum();
            m_hdrOn = m_hdrExposure > 0.f;
        }
        if (out.findKey("blur"))
        {
            m_blurKer = out["blur"].toInt();
            m_blurOn = m_blurKer > 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        throw std::runtime_error("Failed to load render configuration from \"" + filename + "\".");
    }
}

void Renderer::outputPNG()
{
    if (m_bloomOn)
        m_pRenderResult->bloom(m_bloomKer, m_bloomLuminanceThreshold);
    if (m_hdrOn)
        m_pRenderResult->hdr(m_hdrExposure);
    if (m_blurOn)
        m_pRenderResult->blur(m_blurKer);
    m_pRenderResult->outputPNG(m_outputPath, m_scale, m_gamma);
    printf("Render result saved to \"%s\".\n", m_outputPath.c_str());
}