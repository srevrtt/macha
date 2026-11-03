#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;

out vec2 v_uv;

uniform mat4 model;
uniform mat4 projection;

void main() {
    v_uv = aUV;
    gl_Position = projection * model * vec4(aPos, 1.0);
}

