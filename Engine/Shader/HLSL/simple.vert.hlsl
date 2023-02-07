#include "vsoutput.hlsl"

struct simple_vert_input {
  float3 pos : POSITION;
  //   float3 normal : NORMAL;
  //   float2 uv : TEXCOORD;
  //   float3 tangent : TANGENT;
  //   float3 bitangent : BITANGENT;
};

cbuffer PerFrameConstants : register(b0) {
  float4x4 Model; // 64 bytes
  float4x4 View;  // 64 bytes
  float4x4 Proj;  // 64 bytes
};

simple_vert_output vs_main(simple_vert_input i) {
  simple_vert_output o;
  float4 worldPos = mul(Model, float4(i.pos, 1.0));
  o.pos = mul(Proj, mul(View, worldPos));
  o.pos = float4(i.pos, 1.0);

  return o;
}
