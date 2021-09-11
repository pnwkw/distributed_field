#version 450 core

const uint trackPoints = 100;

const float step = 0.1f;

layout (points) in;
layout (points, max_vertices = 256) out;

layout(location = 0) out vec3 outVert;

#include "mag_field/field_glsl_cache_g.glsl"

void main(void) {
    vec3 position = gl_in[0].gl_Position.xyz;

    for(uint i = 0; i < trackPoints; ++i) {
        outVert = position;
        EmitVertex();
        EndPrimitive();

        vec3 b_vec = Field(position);

        position = position - b_vec*step;
    }
}
