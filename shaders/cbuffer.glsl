#ifndef NUM_CASCADES
#define NUM_CASCADES 3
#endif

layout(std140, binding = 0) uniform PerFrameCB {
  mat4 View;
  mat4 PV;
  mat4 PVM;
  vec3 CamPos;
  float perframe_pad0;
  vec3 SunDir;
  float perframe_pad1;
  vec3 LightColor;
  float perframe_pad2;
  vec4 CascadedClipZ;
  mat4 LightPVs[NUM_CASCADES];
};
