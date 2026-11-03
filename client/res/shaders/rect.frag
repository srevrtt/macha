#version 330 core

in vec2 v_uv;
out vec4 fragColor;

uniform vec2 u_size;
uniform float u_radius;
uniform vec3 u_color;

/* signed distance function for a rounded rectangle */
float sd_rounded_box(in vec2 p, in vec2 b, in float r) {
    vec2 q = abs(p) - b + vec2(r);
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
}

void main() {
    /* let's make sure we want to do all of this math first... */
    if (u_radius == 0.0) {
        fragColor = vec4(u_color.rgb, 1.0);
        return;
    }

    /* transform UV coords from [0, 1] into local px space centered at 0,0 */
    vec2 local_pos = (v_uv - 0.5) * u_size;

    /* compute half-extents of the rectangle & distance from the center to the
       edge */
    vec2 half_extents = u_size * 0.5;
    float dist = sd_rounded_box(local_pos, half_extents, u_radius);

    /* smooth edge anti-aliasing */
    float edge_thickness = fwidth(dist);
    float alpha = 1.0 - smoothstep(-edge_thickness, 0.0, dist);

    /* discard or blend transparent fragments */
    if (alpha <= 0.0) {
        discard;
    }

    fragColor = vec4(u_color.rgb, alpha);
}

