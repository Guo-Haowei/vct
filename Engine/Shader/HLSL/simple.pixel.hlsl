#include "vsoutput.hlsl"

float4 ps_main(simple_vert_output i) : SV_TARGET {
  return float4(i.worldPos.rgb, 1.0);
}
