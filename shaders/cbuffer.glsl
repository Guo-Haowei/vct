layout(std140, binding = 0) uniform PerFrameCB {
  mat4 PV;
  mat4 PVM;
  mat4 LightPV;
  vec3 CamPos;
  uint seed;
  vec3 SunDir;
  float perframe_pad1;
  vec3 LightColor;
  float perframe_pad2;
};
