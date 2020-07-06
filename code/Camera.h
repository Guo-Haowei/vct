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

    inline const mat4& getV() const { return m_V; }
    inline const mat4& getP() const { return m_P; }

    inline float getFov() const { return m_fov; }
    inline float getNear() const { return m_zNear; }
    inline float getFar() const { return m_zFar; }

    inline void setFov(float fov) { m_fov = fov; m_projDirty = true; }
    inline void setNear(float zNear) { m_zNear = zNear; m_projDirty = true; }
    inline void setFar(float zFar) { m_zFar = zFar; m_projDirty = true; }

    inline void setSpeed(float speed) { m_speed = speed; }

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
    bool m_viewDirty;
    bool m_projDirty;
    float m_speed; // in scale of world bounding box size?
};
