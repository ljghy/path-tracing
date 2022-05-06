#ifndef RENDERER_H
#define RENDERER_H
#include "path_tracer/Scene.h"
#include "path_tracer/Bitmap.h"
#include "path_tracer/Camera.h"
#include "path_tracer/Shader.h"
#include <cstdint>
#include <string>
#include <memory>

class Renderer
{
private:
    std::shared_ptr<Camera> m_pCamera;
    std::shared_ptr<Shader> m_pShader;
    std::shared_ptr<Bitmap> m_pRenderResult;
    uint16_t m_SPP;

    std::string m_outputPath;
    float m_scale;
    bool m_gamma;

    bool m_bloomOn;
    uint16_t m_bloomKer;
    float m_bloomLuminanceThreshold;

    bool m_hdrOn;
    float m_hdrExposure;

    bool m_blurOn;
    uint16_t m_blurKer;

    void renderThread(Scene *scene, uint16_t offset, uint16_t step);

public:
    Renderer()
        : m_SPP(0u),
          m_outputPath(""),
          m_scale(1.f),
          m_gamma(false),
          m_bloomOn(false),
          m_hdrOn(false),
          m_blurOn(false) {}

    friend void renderThread(Renderer *renderer, Scene *scene, uint8_t id, uint16_t threadNum);
    void render(Scene *scene);
    void loadConfig(const std::string &);
    std::shared_ptr<Bitmap> &renderResult() { return m_pRenderResult; }
    const std::shared_ptr<Bitmap> renderResult() const { return m_pRenderResult; }
    std::string outputPath() const { return m_outputPath; }
    float scale() const { return m_scale; }
    float gamma() const { return m_gamma; }

    void outputPNG();
};
#endif