#include "vsoutput.hlsl"

float4 ps_main(simple_vert_output i) : SV_TARGET {
  float3 n = i.normal;
  n = n + float3(1.0, 1.0, 1.0);
  n = 0.5 * n;
  return float4(n, 1.0);
}
