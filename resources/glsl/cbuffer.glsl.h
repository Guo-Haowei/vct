#ifndef CBUFFER_INCLUDED
#define CBUFFER_INCLUDED

#define NUM_CASCADES    3
#define NUM_SSAO_KERNEL 64

#define MAX_MATERIALS   300
#define MAX_LIGHT_ICON  4
#define MAX_BONE_NUMBER 128

// constant buffer
#ifdef __cplusplus
#define CONSTANT_BUFFER(name, reg) \
    struct name : public ConstantBufferBase<reg>
template<int N>
struct ConstantBufferBase {
    constexpr int get_slot() { return N; }
};
#else
#define CONSTANT_BUFFER(name, reg) layout(std140, binding = reg) uniform name
#endif

// sampler
#ifdef __cplusplus
using sampler2D = uint64_t;
using sampler3D = uint64_t;
typedef struct {
    uint64_t data;
    uint64_t padding;
} Sampler2DArray;
#else
#define Sampler2DArray sampler2D
#endif

CONSTANT_BUFFER(PerFrameConstantBuffer, 0) {
    mat4 c_view_matrix;
    mat4 c_projection_matrix;
    mat4 c_projection_view_matrix;

    vec3 c_camera_position;
    int c_debug_csm;

    vec3 c_sun_direction;
    int c_enable_vxgi;

    vec3 c_light_color;
    float c_voxel_size;

    vec4 c_cascade_clip_z;
    mat4 c_light_matricies[NUM_CASCADES];

    vec3 c_world_center;
    float c_world_size_half;

    int c_debug_texture_id;
    int c_no_texture;
    int c_screen_width;
    int c_screen_height;

    int c_ssao_kernel_size;
    float c_ssao_kernel_radius;
    int c_ssao_noise_size;
    float c_texel_size;

    vec2 padding0;
    int c_enable_ssao;
    int c_enable_fxaa;
};

CONSTANT_BUFFER(PerBatchConstantBuffer, 1) {
    mat4 c_projection_view_model_matrix;
    mat4 c_model_matrix;
};

CONSTANT_BUFFER(MaterialConstantBuffer, 2) {
    vec4 AlbedoColor;
    float Metallic;
    float Roughness;
    int HasAlbedoMap;
    int HasPbrMap;

    int HasNormalMap;
    int TextureMapIdx;
    float ReflectPower;
    int _padint0;
};

CONSTANT_BUFFER(PerSceneConstantBuffer, 3) {
    vec4 SSAOKernels[NUM_SSAO_KERNEL];
    sampler2D c_shadow_map;
    sampler3D c_voxel_map;
    sampler3D c_voxel_normal_map;
    sampler3D c_voxel_emissive_map;
    sampler2D c_gbuffer_albedo_map;
    sampler2D c_gbuffer_position_metallic_map;
    sampler2D c_gbuffer_normal_roughness_map;
    sampler2D c_gbuffer_depth_map;
    sampler2D c_ssao_map;
    sampler2D c_kernel_noise_map;
    sampler2D c_fxaa_image;
    sampler2D c_fxaa_input_image;
    Sampler2DArray LightIconTextures[MAX_LIGHT_ICON];
    Sampler2DArray AlbedoMaps[MAX_MATERIALS];
    Sampler2DArray NormalMaps[MAX_MATERIALS];
    Sampler2DArray PbrMaps[MAX_MATERIALS];
};

CONSTANT_BUFFER(BoneConstantBuffer, 4) {
    mat4 c_bones[MAX_BONE_NUMBER];
};

#endif