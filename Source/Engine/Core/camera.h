#pragma once
#include "universal/core_math.h"

class Camera {
   public:
    vec3 direction() const;

    float fovy;
    float zNear;
    float zFar;
    float yaw   = -90.0f;
    float pitch = 0.0f;
    vec3 position{ 0 };

    void UpdatePV();

    inline void SetAspect( float aspect ) { mAspect = aspect; }
    inline float GetAspect() const { return mAspect; }

    inline const mat4& Proj() const { return mProj; }
    inline const mat4& View() const { return mView; }
    inline const mat4& ProjView() const { return mProjView; }

   private:
    float mAspect;
    mat4 mProj;
    mat4 mView;
    mat4 mProjView;
};
