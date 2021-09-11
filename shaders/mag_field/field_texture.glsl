layout(binding = 0) uniform sampler3D fieldTexture;

#include "measurements.glsl"

vec3 world_to_uv(vec3 position) {
    return (position.zxy + offsets) / detector_size;
}

vec3 Field(vec3 pos) {
    vec3 uv = world_to_uv(pos);
    return textureLod(fieldTexture, uv, 0).xyz;
}
