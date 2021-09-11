#version 460 core

layout(location = 0) in vec2 inVert;
layout(location = 0) out vec3 outPos;

layout(std140, binding = 0) uniform state_state {
    mat4 MVP;
} state;

void main(void) {
    gl_Position = state.MVP * vec4(inVert, 0.0f, 1.0f);
    outPos = vec3(inVert.y, 0.0f, inVert.x);
}
