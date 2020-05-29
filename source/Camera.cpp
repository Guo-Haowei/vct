#include "Camera.h"
#include "InputManager.h"
#include <iostream>

PerspectiveCamera::PerspectiveCamera(float fov, float zNear, float zFar)
    : m_fov(fov)
    , m_zNear(zNear)
    , m_zFar(zFar)
    , m_aspect(0.0f)
    , m_viewDirty(true)
    , m_projDirty(true)
    , m_M(glm::MAT4_IDENTITY)
    , m_speed(1.0f)
{
}

void PerspectiveCamera::update()
{
    InputManager& im = InputManager::getInstance();
    if (im.isButtonDown(InputManager::BUTTON_LEFT))
    {
        float rotation = static_cast<float>(im.getCursorDelta().x);
        if (rotation != 0)
        {
            float rFactor = -0.0003f;
            // rotate camera around it's world y axis
            mat4 T = glm::translate(glm::MAT4_IDENTITY, -center());
            mat4 R = glm::rotate(glm::MAT4_IDENTITY, rFactor * rotation, glm::VEC3_UNIT_Y);
            mat4 T_1 = glm::translate(glm::MAT4_IDENTITY, center());
            m_M = T_1 * R * T * m_M;
            m_viewDirty = true;
        }
    }
    else if (im.isButtonDown(InputManager::BUTTON_RIGHT))
    {
        static const float panSensitivity = 0.002f;
        dvec2 displacement = im.getCursorDelta();
        if (displacement.x == 0 && displacement.y == 0)
            return;

        float dirX = static_cast<float>(displacement.x) * m_speed * panSensitivity;
        float dirY = static_cast<float>(displacement.y) * m_speed * panSensitivity;
        vec3 t = dirX * right() + dirY * glm::VEC3_UNIT_Y;
        mat4 T = glm::translate(glm::MAT4_IDENTITY, t);
        m_M = T * m_M;
        m_viewDirty = true;
        // pan camera
    }
    else if (im.getScroll() != 0.0f)
    {
        static const float scrollSensitivity = 0.2f;
        float distance = im.getScroll() * scrollSensitivity * m_speed;
        vec3 t = distance * front();
        mat4 T = glm::translate(glm::MAT4_IDENTITY, t);
        m_M = T * m_M;
        m_viewDirty = true;
    }

    if (m_viewDirty)
    {
        // update view matrix
        m_V = glm::lookAt(center(), center() + front(), up());
        m_viewDirty = false;
    }
    if (m_projDirty)
    {
        // use orthographic camera
        float h = 1.0f;
        float w = m_aspect * h;
        // m_P = glm::ortho(-w, w, -h, h);
        m_P = glm::perspective(m_fov, m_aspect, m_zNear, m_zFar);
        m_projDirty = false;
    }
}

void PerspectiveCamera::moveFront(float d)
{
    m_M = glm::translate(m_M, d * front());
    m_viewDirty = true;
}

void PerspectiveCamera::moveUp(float d)
{
    m_M = glm::translate(m_M, d * up());
    m_viewDirty = true;
}

void PerspectiveCamera::moveRight(float d)
{
    m_M = glm::translate(m_M, d * right());
    m_viewDirty = true;
}

void PerspectiveCamera::setAspect(float aspect)
{
    if (m_aspect != aspect && !glm::isnan(aspect) && aspect != 0.0f)
    {
        m_aspect = aspect;
        m_projDirty = true;
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
