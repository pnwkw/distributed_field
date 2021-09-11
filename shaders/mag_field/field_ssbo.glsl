#include "measurements.glsl"

layout(std430, binding = 1) buffer storage_block {
    vec4 data[];
};

const ivec3 detector_dimensions_scaled = ivec3(detector_size / scales);

int world_to_index(vec3 position) {
    ivec3 r = ivec3((position.zxy + offsets) / scales);

    return (r.p * detector_dimensions_scaled.s * detector_dimensions_scaled.t) + r.t * detector_dimensions_scaled.s + r.s;
}

vec3 Field(vec3 pos) {
    int index = world_to_index(pos);
    return data[index].xyz;
}
