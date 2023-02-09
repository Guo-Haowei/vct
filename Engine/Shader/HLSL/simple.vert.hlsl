#include "../../Graphics/cbuffer.shader.hpp"
#include "vsoutput.hlsl"

struct simple_vert_input {
  float3 pos : POSITION;
  float3 normal : NORMAL;
  //   float2 uv : TEXCOORD;
  //   float3 tangent : TANGENT;
  //   float3 bitangent : BITANGENT;
};

simple_vert_output vs_main(simple_vert_input i) {
  simple_vert_output o;
  float4 worldPos = mul(Model, float4(i.pos, 1.0));
  o.pos = mul(Proj, mul(View, worldPos));
  o.worldPos = worldPos.xyz;

  float4 normal4 = mul(Model, float4(i.normal, 0.0));
  o.normal = normalize(normal4.xzy);
  return o;
}
