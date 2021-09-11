#version 460 core

layout(location = 0) in vec3 inVert;
layout(location = 0) out vec3 outVert;

#include "mag_field/field_glsl_cache_v.glsl"

void main(void) {
    outVert = Field(inVert);
}
