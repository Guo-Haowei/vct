#pragma once
#include "math/GeoMath.h"

// TODO: move camera to scene
class PerspectiveCamera
{
public:
    PerspectiveCamera(float fov = glm::QUARTER_PI_F, float zNear = 0.1f, float zFar = 1000.0f);

    void update();

    void moveFront(float d);
    void moveUp(float d);
    void moveRight(float d);

    const mat4& getV() const { return m_V; }
    const mat4& getP() const { return m_P; }

    void setAspect(float aspect);

    template<typename T>
    inline void setAspect(T width, T height)
    {
        setAspect(static_cast<float>(width) / static_cast<float>(height));
    }

private:
    vec3 right();
    vec3 up();
    vec3 front();
    vec3 center();

    mat4 m_M;
    mat4 m_V;
    mat4 m_P;
    float m_fov;
    float m_aspect;
    float m_zNear;
    float m_zFar;
    bool m_dirty;
    float m_speed; // in scale of world bounding box size?
};
