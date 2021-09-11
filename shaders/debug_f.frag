#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 0) out vec3 outColor;

#include "mag_field/field_ssbo.glsl"

void main(void) {
    outColor = Field(inPos);
}
