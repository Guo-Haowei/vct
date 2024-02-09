#ifndef VSINPUT_INCLUDED
#define VSINPUT_INCLUDED

#if __cplusplus
#define BUFFER_SLOT(type, name, slot) \
    constexpr int get_##name##_slot() { return slot; }

#else
#define BUFFER_SLOT(type, name, slot) layout(location = slot) in type in_##name;
#endif
BUFFER_SLOT(vec3, position, 0)
BUFFER_SLOT(vec3, normal, 1)
BUFFER_SLOT(vec2, uv, 2)
BUFFER_SLOT(vec3, tangent, 3)
BUFFER_SLOT(ivec4, bone_id, 4)
BUFFER_SLOT(vec4, bone_weight, 5)
#endif
