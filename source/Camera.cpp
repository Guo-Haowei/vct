#include "Camera.h"

PerspectiveCamera::PerspectiveCamera(float fov, float zNear, float zFar)
    : m_fov(fov)
    , m_zNear(zNear)
    , m_zFar(zFar)
    , m_aspect(0.0f)
    , m_dirty(true)
    , m_M(glm::MAT4_IDENTITY)
    , m_speed(1.0f)
{
}

void PerspectiveCamera::update()
{
    if (m_dirty)
    {
        // update view matrix
        m_V = glm::lookAt(center(), center() + front(), up());
        m_dirty = false;
    }
}

void PerspectiveCamera::moveFront(float d)
{
    m_M = glm::translate(m_M, d * front());
    m_dirty = true;
}

void PerspectiveCamera::moveUp(float d)
{
    m_M = glm::translate(m_M, d * up());
    m_dirty = true;
}

void PerspectiveCamera::moveRight(float d)
{
    m_M = glm::translate(m_M, d * right());
    m_dirty = true;
}

void PerspectiveCamera::setAspect(float aspect)
{
    if (m_aspect != aspect)
    {
        m_aspect = aspect;
        m_P = glm::perspective(m_fov, m_aspect, m_zNear, m_zFar);
    }
}

vec3 PerspectiveCamera::right()
{
    return vec3(m_M[0]);
}

vec3 PerspectiveCamera::up()
{
    return vec3(m_M[1]);
}

vec3 PerspectiveCamera::front()
{
    return -vec3(m_M[2]);
}

vec3 PerspectiveCamera::center()
{
    return vec3(m_M[3]);
}
