#pragma once
#include "GpuResource.h"
#include "GpuBuffer.h"

class VertexArray : public GpuResource
{
public:
    struct CreateInfo
    {
        GLenum primitive; // GL_TRIANGLES, GL_LINES
    };

    struct AttribInfo
    {
        GLenum dataType; // GL_FLOAT, GL_UNSIGNED_INT
        int dataDimension; // 1, 2, 3 or 4
        int stride;
        size_t offset = 0;
        int divisor = 0;
    };

    struct InstanceInfo
    {
        int divisor = 1;
    };

    struct IndexInfo
    {
        GLenum dataType; // GL_UNSIGNED_INT, GL_UNSIGNED_SHORT, etc...
    };

    VertexArray(const std::string& debugName, const CreateInfo& createInfo);

    // vao.bind().appendAttribute().appendAttribute()...appendIndexBuffer().unbind();
    VertexArray& appendAttribute(const AttribInfo& info, const GpuBuffer& vertexBuffer);
    VertexArray& appendIndexBuffer(const IndexInfo& info, const GpuBuffer& indexBuffer);
    VertexArray& bind();
    VertexArray& unbind();
protected:
    virtual void internalRelease() override;

    // VertexArray is not responsible for releasing buffers
    unsigned int m_attribsCount;

    GLenum m_primitive;
    GLenum m_indexDataType;
}; 
