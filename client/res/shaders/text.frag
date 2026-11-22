#version 330 core

in vec2 v_uv;
out vec4 fragColor;

uniform sampler2D text;
uniform vec3 f_textColor;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, v_uv).r);
    fragColor = vec4(f_textColor, 1.0) * sampled;
}

