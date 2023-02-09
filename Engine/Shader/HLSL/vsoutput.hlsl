struct simple_vert_output {
  float4 pos : SV_POSITION;
  float3 worldPos : POSITION; // pack metallic in w component
  //   float3 normal : NORMAL;     // pack roughess in w component
  //   float2 uv : TEXCOORD;
  //   float3x3 TBN : TBN;
};
