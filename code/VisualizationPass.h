#include "common.h"
#include "GL/ShaderProgram.h"
#include "GL/VertexArray.h"
#include <memory>

class VisualizationPass
{
public:
    void initialize();
    void render();
    void finalize();
private:
    void clearTexture();

    std::unique_ptr<ShaderProgram> m_visualizationShader;
    std::unique_ptr<VertexArray> m_boxVao;
    std::unique_ptr<GpuBuffer> m_boxVbo;
    std::unique_ptr<GpuBuffer> m_boxEbo;
};
