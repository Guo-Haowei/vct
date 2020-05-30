#include "VertexArray.h"
#include <assert.h>

VertexArray::VertexArray(const std::string& debugName, const CreateInfo& info)
    : GpuResource(debugName)
    , m_attribsCount(0)
    , m_primitive(info.primitive)
    , m_indexDataType(GL_UNSIGNED_INT) // default to GL_UNSIGNED_INT
{
    glCreateVertexArrays(1, &m_handle);
}

VertexArray& VertexArray::appendAttribute(const AttribInfo& info, const GpuBuffer& vertexBuffer)
{
    assert(vertexBuffer.getType() == GL_ARRAY_BUFFER);
    glBindBuffer(vertexBuffer.getType(), vertexBuffer.getHandle());
    auto location = m_attribsCount++;
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, info.dataDimension, info.dataType, GL_FALSE, info.stride, (void*)info.offset);
    if (info.divisor != 0) glVertexAttribDivisor(location, info.divisor);
    glBindBuffer(vertexBuffer.getType(), 0);
    return *this;
}

VertexArray& VertexArray::appendIndexBuffer(const IndexInfo& info, const GpuBuffer& indexBuffer)
{
    // m_handle has to be bound
    assert(indexBuffer.getType() == GL_ELEMENT_ARRAY_BUFFER);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.getHandle());
    m_indexDataType = info.dataType;
    return *this;
}

VertexArray& VertexArray::bind()
{
    glBindVertexArray(m_handle);
    return *this;
}

VertexArray& VertexArray::unbind()
{
    glBindVertexArray(0);
    return *this;
}

void VertexArray::internalRelease()
{
    glDeleteVertexArrays(1, &m_handle);
}
