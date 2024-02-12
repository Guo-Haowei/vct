#include "cbuffer.glsl.h"

layout(location = 0) in vec2 pass_uv;
layout(location = 0) out float occlusion;

#include "common.glsl"

void main() {
    const float bias = 0.01;

    vec2 noiseScale = vec2(float(c_screen_width), float(c_screen_height));
    noiseScale /= float(c_ssao_noise_size);

    const vec2 uv = pass_uv;
    const vec3 N = normalize(texture(c_gbuffer_normal_roughness_map, uv).xyz);
    const vec3 rvec = texture(c_kernel_noise_map, noiseScale * uv).xyz;
    const vec3 tangent = normalize(rvec - N * dot(rvec, N));
    const vec3 bitangent = cross(N, tangent);

    mat3 TBN = mat3(tangent, bitangent, N);
    TBN = mat3(c_view_matrix) * TBN;

    vec4 origin = vec4(texture(c_gbuffer_position_metallic_map, uv).xyz, 1.0);
    origin = c_view_matrix * origin;

    occlusion = 0.0;
    for (int i = 0; i < c_ssao_kernel_size; ++i) {
        // get sample position
        vec3 samplePos = TBN * c_ssao_kernels[i].xyz;  // from tangent to view-space
        samplePos = origin.xyz + samplePos * c_ssao_kernel_radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = c_projection_matrix * offset;  // from view to clip-space
        offset.xyz /= offset.w;                 // perspective divide
        offset.xy = offset.xy * 0.5 + 0.5;      // transform to range 0.0 - 1.0

        // get sample depth
        const vec4 samplec_view_matrixSpace =
            c_view_matrix * vec4(texture(c_gbuffer_position_metallic_map, offset.xy).xyz, 1.0);  // get depth value of kernel sample
        const float sampleDepth = samplec_view_matrixSpace.z;

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, c_ssao_kernel_radius / abs(origin.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / float(c_ssao_kernel_size));
}
